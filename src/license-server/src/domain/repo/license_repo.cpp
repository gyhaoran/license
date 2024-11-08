#include "domain/repo/license_repo.h"
#include <iostream>

namespace lic
{

void LicenseRepo::add_device(const DeviceId& id, const DeviceInfo& info)
{
    devices_[id] = info;
}

void LicenseRepo::remove_device(const DeviceId& id)
{
    devices_.erase(id);
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
        rsp["status"] = "Failure";
        rsp["message"] = "Device not authorized";
        return false;
    }

    DeviceInfo& device = iter->second;
    if (device.current_instance >= device.max_instance)
    {
        rsp["status"] = "Failure";
        rsp["message"] = "Max instance limit reached";
        return false;
    }
    
    ++device.current_instance;
    if (!period_.is_valid())
    {
        rsp["status"] = "Failure";
        rsp["message"] = "License has expired";
        return false;
    }

    rsp["status"] = "Success";
    rsp["message"] = "Authorization successful";
    rsp["deviceid"] = device_id;
    return true;
}

void LicenseRepo::clear()
{
    devices_.clear();
}

} // namespace lic
