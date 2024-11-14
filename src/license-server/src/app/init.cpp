#include "app/init.h"
#include "app/config/env_parser.h"
#include "domain/repo/license_repo.h"
#include "domain/entities/license_period.h"
#include "infra/log/log.h"
#include "service/serialization_service.h"
#include "license_parser.h"
#include "hash.h"
#include "json.hpp"
#include <iostream>
#include <csignal>
#include <hv/hlog.h>

namespace lic
{

std::map<std::string, int> parse_json_object(const nlohmann::json& msg) 
{
    std::map<std::string, int> result;

    if (msg.is_object()) 
    {
        for (const auto& [key, value] : msg.items()) 
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
    save_to_file(LicenseRepo::get_instance().devices(), EnvParser::get_data_path());
    std::exit(signum);
}

void reg_signal()
{
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
}

void init_log()
{
    hlog_set_level(LOG_LEVEL_ERROR);
    hlog_set_file("/tmp/.iCell/logs/lic-service-end.log");
}

void init()
{
    init_log();

    auto license_info = get_license_info(EnvParser::get_license_path());
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
    if (load_from_file(EnvParser::get_data_path(), device_infos))
    {
        inst.recover_devices(device_infos);
    }

    reg_signal();
}

} // namespace lic
