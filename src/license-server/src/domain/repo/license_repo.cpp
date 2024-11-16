#include "domain/repo/license_repo.h"
#include "domain/msg/auth_req_msg.h"
#include "infra/log/log.h"
#include "device.h"
#include <iostream>

namespace lic
{

namespace
{

void build_auth_rsp_msg(::nlohmann::json& rsp, const std::string& message, const InstanceId& instance_id="", bool success=false)
{
    if (success)
    {
        rsp["status"] = "Success";
        rsp["uuid"] = instance_id;
    }
    else
    {
        rsp["status"] = "Failure";
    }
    rsp["message"] = message;
}

}

void LicenseRepo::add_device(const DeviceId& id)
{
    devices_.emplace(id);
}

void LicenseRepo::remove_device(const DeviceId& id)
{
    devices_.erase(id);
}

void LicenseRepo::recover_devices(const InstanceInfos& insts)
{
    std::lock_guard<std::mutex> lock(mutex_);
    instances_ = insts;
}

void LicenseRepo::set_license_period(const LicensePeriod& period)
{
    period_ = period;
}

void LicenseRepo::set_max_instance(int num)
{
    max_inst_num_ = num;
}

bool LicenseRepo::validate(const AuthReqMsg& req, ::nlohmann::json& rsp)
{
    auto& cpu_id = req.cpu_id;
    for (const auto& mac : req.mac_addresses)
    {
        auto device_id = gen_device_hash(cpu_id, mac);
        if (devices_.contains(device_id))
        {
            return check(req.instance_id, rsp);
        }
    }
    LOG_WARN("Device not authorized");
    build_auth_rsp_msg(rsp, "Device not authorized");
    return false;
}

bool LicenseRepo::check(const InstanceId& instance_id, ::nlohmann::json& rsp)
{
    if (instances_.size() >= max_inst_num_)
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

    add_instance(instance_id);

    build_auth_rsp_msg(rsp, "License activate success", instance_id, true);
    return true;
}

void LicenseRepo::add_instance(const InstanceId& instance_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instances_.contains(instance_id))
    {
        instances_[instance_id].last_heartbeat = std::chrono::steady_clock::now();
    }
    else
    {
        instances_[instance_id] = InstanceInfo{instance_id, std::chrono::steady_clock::now()};
    }
}

void LicenseRepo::release_instance(const InstanceId& instance_id)
{
    if (!instances_.contains(instance_id))
    {
        LOG_WARN("rel req, rcv invalid instance_id: %s", instance_id.c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    instances_.erase(instance_id);
}

void LicenseRepo::update_instance(const InstanceId& instance_id)
{
    add_instance(instance_id);
}

void LicenseRepo::remove_inactive_inst(int timeout)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = instances_.begin(); it != instances_.end();) 
    {
        auto last_heartbeat = it->second.last_heartbeat;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_heartbeat).count();
        int64_t timeout_us = timeout * 1000 * 1000;
        if (duration > timeout_us) 
        {
            LOG_INFO("inst_id: %s, duration %lld, timeout: %lld", it->first.c_str(), duration, timeout_us);
            it = instances_.erase(it);
        } 
        else
        {
            ++it;
        }
    }
}

void LicenseRepo::clear_instances()
{
    instances_.clear();
}

void LicenseRepo::clear()
{
    max_inst_num_ = 1;
    period_ = LicensePeriod();
    devices_.clear();
    instances_.clear();
}

void LicenseRepo::dump() const
{
    std::cout << "LicenseRepo info:\n";
    std::cout << "Max instance num: " << max_inst_num_ << '\n';
    std::cout << "Cur instance num: " << instances_.size() << '\n';
    std::cout << "InstanceInfos: [";
    for (auto& [id, inst] : instances_)
    {
        std::cout << inst.to_string() << ", ";
    }
    std::cout << "]\n";
}

InstanceInfos& LicenseRepo::instances()
{
    return instances_;
}

DeviceInfos LicenseRepo::devices() const
{
    return devices_;
}

} // namespace lic
