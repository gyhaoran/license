#ifndef ECE16A72_2654_4224_A2C6_025F0E6C922A
#define ECE16A72_2654_4224_A2C6_025F0E6C922A

#include "domain/entities/device_info.h"
#include "domain/entities/license_period.h"
#include "infra/singleton.h"
#include "json.hpp"
#include <set>

namespace lic
{

struct AuthReqMsg;
struct LicensePeriod;
using DeviceInfos = std::set<DeviceId>;

DEF_SINGLETON(LicenseRepo)
{
    void add_device(const DeviceId&);
    void remove_device(const DeviceId&);

    void add_instance(const InstanceId&);
    void release_instance(const InstanceId&);

    void recover_devices(const InstanceInfos&);
    void set_license_period(const LicensePeriod&);

    void set_max_instance(int);

    bool validate_auth_req(const AuthReqMsg&, ::nlohmann::json&);
    void update_instance(const InstanceId&);

    void clear_instances();
    void clear();

    void remove_inactive_inst(int timeout);

    InstanceInfos& instances();
    DeviceInfos devices() const;

    void dump() const;
private:
    bool validate(::nlohmann::json&);

private:
    int max_inst_num_{1};
    LicensePeriod period_{};
    DeviceInfos devices_{};

    InstanceInfos instances_{};
    std::mutex mutex_;
};
    
} // namespace lic


#endif /* ECE16A72_2654_4224_A2C6_025F0E6C922A */
