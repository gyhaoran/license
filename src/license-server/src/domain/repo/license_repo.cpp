#include "domain/repo/license_repo.h"
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

void LicenseRepo::reset_devices(const DeviceInfos& devices)
{
    devices_ = devices;
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

bool LicenseRepo::validate(const DeviceId& device_id, ::nlohmann::json& rsp)
{
    auto iter = devices_.find(device_id);
    if (iter == devices_.end())
    {
        build_auth_rsp_msg(rsp, "Device not authorized");
        return false;
    }

    DeviceInfo& device = iter->second;
    if (device.current_instance >= device.max_instance)
    {
        build_auth_rsp_msg(rsp, "Max instance limit reached");
        return false;
    }
    
    ++device.current_instance;
    if (!period_.is_valid())
    {
        build_auth_rsp_msg(rsp, "License has expired");
        return false;
    }

    build_auth_rsp_msg(rsp, "License has expired", device_id, true);
    return true;
}

void LicenseRepo::clear()
{
    devices_.clear();
}

DeviceInfos& LicenseRepo::devices()
{
    return devices_;
}

} // namespace lic
