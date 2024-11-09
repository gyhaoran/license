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

void LicenseRepo::release_inst(const DeviceId& device_id)
{
    auto iter = devices_.find(device_id);
    if (iter != devices_.end())
    {
        --iter->second.current_instance;
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
            return check(device_id, rsp);
        }
    }
    build_auth_rsp_msg(rsp, "Device not authorized");
}

bool LicenseRepo::check(const DeviceId& device_id, ::nlohmann::json& rsp)
{
    auto& device = devices_[device_id];
    if (device.current_instance >= device.max_instance)
    {
        LOG_ERROR("Max instance limit reached");
        build_auth_rsp_msg(rsp, "Max instance limit reached");
        return false;
    }
    
    ++device.current_instance;
    if (!period_.is_valid())
    {
        LOG_ERROR("License has expired");
        build_auth_rsp_msg(rsp, "License has expired");
        return false;
    }

    build_auth_rsp_msg(rsp, "License activate success", device_id, true);
    return true;
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
        std::cout << "Device Id: " << id << "\nDeviceInfo: " << value.to_string() << '\n';
    }
}

DeviceInfos& LicenseRepo::devices()
{
    return devices_;
}

} // namespace lic
