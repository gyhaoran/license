#ifndef ECE16A72_2654_4224_A2C6_025F0E6C922A
#define ECE16A72_2654_4224_A2C6_025F0E6C922A

#include "domain/device_info.h"
#include "domain/license_period.h"
#include "infra/singleton.h"
#include "json.hpp"
#include <map>

namespace lic
{

struct LicensePeriod;
using DeviceInfos = std::map<DeviceId, DeviceInfo>;

DEF_SINGLETON(LicenseRepo)
{
    void add_device(const DeviceId&, const DeviceInfo&);
    void remove_device(const DeviceId&);
    void recover_devices(const DeviceInfos&);

    void release_inst(const DeviceId&);

    void set_license_period(const LicensePeriod&);

    bool validate(const DeviceId&, ::nlohmann::json&);

    void clear();

    DeviceInfos& devices();

private:
    LicensePeriod period_{};
    DeviceInfos devices_{};
};
    
} // namespace lic


#endif /* ECE16A72_2654_4224_A2C6_025F0E6C922A */
