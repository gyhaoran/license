#include "domain/repo/license_repo.h"
#include "domain/msg/auth_req_msg.h"
#include "infra/log/log.h"
#include "device.h"
#include <iostream>

namespace lic
{

namespace
{

void build_auth_rsp_msg(::nlohmann::json& rsp, const std::string& message, const DeviceId& device_id="", bool success=false)
{
    if (success)
    {
        rsp["status"] = "Success";
        rsp["deviceid"] = device_id;
    }
    else
    {
        rsp["status"] = "Failure";
    }
    rsp["message"] = message;
}

}

void LicenseRepo::add_device(const DeviceId& id, const DeviceInfo& info)
{
    devices_[id] = info;
}

void LicenseRepo::remove_device(const DeviceId& id)
{
    devices_.erase(id);
}

void LicenseRepo::recover_devices(const DeviceInfos& devices)
{
    for (const auto& [id, value] : devices) 
    {
        if (auto it = devices_.find(id); it != devices_.end()) 
        {
            it->second = value;
        }
    }
}

 void LicenseRepo::set_license_period(const LicensePeriod& period)
 {
    period_ = period;
 }

bool LicenseRepo::validate(const AuthReqMsg& req, ::nlohmann::json& rsp)
{
    auto& cpu_id = req.cpu_id;
    for (const auto& mac : req.mac_addresses)
    {
        auto device_id = gen_device_hash(cpu_id, mac);

        auto iter = devices_.find(device_id);
        if (iter == devices_.end())
        {
            continue;
        }
        else
        {
            return check(device_id, req.instance_id, rsp);
        }
    }
    LOG_WARN("Device not authorized");
    build_auth_rsp_msg(rsp, "Device not authorized");
    return false;
}

bool LicenseRepo::check(const DeviceId& device_id, const InstanceId& instance_id, ::nlohmann::json& rsp)
{
    auto& device = devices_[device_id];
    if (device.instances.size() >= device.max_instance)
    {
        LOG_ERROR("Max instance limit reached");
        build_auth_rsp_msg(rsp, "Max instance limit reached");
        return false;
    }
    
    if (!period_.is_valid())
    {
        LOG_ERROR("License has expired");
        build_auth_rsp_msg(rsp, "License has expired");
        return false;
    }

    add_instance(device_id, instance_id);

    build_auth_rsp_msg(rsp, "License activate success", device_id, true);
    return true;
}

void LicenseRepo::add_instance(const DeviceId& device_id, const InstanceId& instance_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& device = devices_[device_id];
    if (device.instances.contains(instance_id))
    {
        device.instances[instance_id].last_heartbeat = std::chrono::steady_clock::now();
    }
    else
    {
        device.instances[instance_id] = InstanceInfo{instance_id, std::chrono::steady_clock::now()};
    }
}

void LicenseRepo::release_instance(const DeviceId& device_id, const InstanceId& instance_id)
{
    if (!devices_.contains(device_id))
    {
        LOG_WARN("rel req, rcv invalid device_id: %s", device_id.c_str());
        return;
    }

    auto& device = devices_[device_id];
    if (device.instances.contains(instance_id))
    {
        std::lock_guard<std::mutex> lock(mutex_);
        device.instances.erase(instance_id);
    }
}

void LicenseRepo::update_instance(const DeviceId& device_id, const InstanceId& instance_id)
{
    if (!devices_.contains(device_id))
    {
        LOG_WARN("rcv invalid device_id: %s", device_id.c_str());
        return;
    }
    
    add_instance(device_id, instance_id);
}

void LicenseRepo::remove_inactive_inst(int timeout)
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [device_id, device] : devices_) 
    {
        for (auto it = device.instances.begin(); it != device.instances.end(); ) 
        {
            auto last_heartbeat = it->second.last_heartbeat;
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat).count();

            LOG_INFO("inst_id: %s, duration %lld, timeout: %d", it->first.c_str(), duration, timeout);

            if (duration > timeout) 
            {
                it = device.instances.erase(it);
            } 
            else 
            {
                ++it;
            }
        }
    }
}

void LicenseRepo::clear()
{
    devices_.clear();
}

void LicenseRepo::dump()
{
    std::cout << "LicenseRepo info:\n";
    for (const auto& [id, value] : devices_) 
    {
        std::cout << "DeviceInfo: " << value.to_string() << '\n';
    }
}

DeviceInfos& LicenseRepo::devices()
{
    return devices_;
}

} // namespace lic
