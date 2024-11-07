#include "domain/repo/license_repo.h"

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

 void LicenseRepo::set_license_period(const LicensePeriod& period)
 {
    period_ = period;
 }

void LicenseRepo::validate()
{
    
}

void LicenseRepo::clear()
{
    devices_.clear();
}

} // namespace lic
