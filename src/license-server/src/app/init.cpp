#include "app/init.h"
#include "domain/repo/license_repo.h"
#include "domain/entities/license_period.h"
#include "infra/log/log.h"
#include "service/serialization_service.h"
#include "license_parser.h"
#include "hash.h"
#include "json.hpp"
#include <iostream>
#include <csignal>

namespace lic
{

std::map<std::string, int> parse_json_object(const nlohmann::json& j) 
{
    std::map<std::string, int> result;

    if (j.is_object()) 
    {
        for (const auto& [key, value] : j.items()) 
        {
            if (value.is_number_integer()) 
            {
                result[key] = value.get<int>();
            }
            else 
            {
                LOG_ERROR("Value for key: %s is not an integer.", key.c_str());
            }
        }
    }
    else 
    {
        LOG_ERROR("The provided JSON is not an object");
    }

    return result;
}

void signal_handler(int signum) 
{
    LOG_ERROR("Rcv signal: %d", signum);
    save_to_file(LicenseRepo::get_instance().devices(), "./info.dat");
    std::exit(signum);
}

void reg_signal()
{
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
}

void init()
{
    auto license_info = get_license_info("./license.dat");
    auto info = nlohmann::json::parse(license_info);

    auto issue_date = info["issue_date"];
    auto expire_date = info["expire_date"];

    auto& inst = LicenseRepo::get_instance();
    inst.set_license_period(LicensePeriod(issue_date, expire_date));

    auto all_devices = parse_json_object(info["devices"]);
    for (auto& [device_id, max_inst] : all_devices)
    {
        inst.add_device(device_id, DeviceInfo(device_id, max_inst));
    }

    DeviceInfos device_infos{};
    if (load_from_file("./info.dat", device_infos))
    {
        inst.recover_devices(device_infos);
    }

    reg_signal();
}

} // namespace lic
