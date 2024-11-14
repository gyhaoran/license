#ifndef ECE16A72_2654_4224_A2C6_025F0E6C922A
#define ECE16A72_2654_4224_A2C6_025F0E6C922A

#include "domain/entities/device_info.h"
#include "domain/entities/license_period.h"
#include "infra/singleton.h"
#include "json.hpp"
#include <map>

namespace lic
{

struct AuthReqMsg;
struct LicensePeriod;
using DeviceInfos = std::map<DeviceId, DeviceInfo>;

DEF_SINGLETON(LicenseRepo)
{
    void add_device(const DeviceId&, const DeviceInfo&);
    void remove_device(const DeviceId&);

    void add_instance(const DeviceId&, const InstanceId&);
    void release_instance(const DeviceId&, const InstanceId&);

    void recover_devices(const DeviceInfos&);
    void set_license_period(const LicensePeriod&);

    bool validate(const AuthReqMsg&, ::nlohmann::json&);
    void update_instance(const DeviceId&, const InstanceId&);

    void clear_device();
    void clear();
    void dump();

    void remove_inactive_inst(int timeout);

    DeviceInfos& devices();

private:
    bool check(const DeviceId&, const InstanceId&, ::nlohmann::json&);

private:
    LicensePeriod period_{};
    DeviceInfos devices_{};
    std::mutex mutex_;
};
    
} // namespace lic


#endif /* ECE16A72_2654_4224_A2C6_025F0E6C922A */
