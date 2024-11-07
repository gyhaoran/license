#ifndef ECE16A72_2654_4224_A2C6_025F0E6C922A
#define ECE16A72_2654_4224_A2C6_025F0E6C922A

#include "domain/device_info.h"
#include "domain/license_period.h"
#include "infra/singleton.h"
#include <map>

namespace lic
{

struct LicensePeriod;

DEF_SINGLETON(LicenseRepo)
{
    void add_device(const DeviceId&, const DeviceInfo&);
    void remove_device(const DeviceId&);

    void set_license_period(const LicensePeriod& period);
    
    void validate();

    void clear();

private:
    LicensePeriod period_{};
    std::map<DeviceId, DeviceInfo> devices_{};
};
    
} // namespace lic


#endif /* ECE16A72_2654_4224_A2C6_025F0E6C922A */