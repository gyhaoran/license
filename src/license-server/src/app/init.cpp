#include "app/init.h"
#include "app/config/env_parser.h"
#include "domain/repo/license_repo.h"
#include "domain/entities/license_period.h"
#include "infra/log/log.h"
#include "service/serialization_service.h"
#include "license_parser.h"
#include "hash.h"
#include "json.hpp"
#include <hv/hlog.h>
#include <iostream>
#include <csignal>
#include <vector>

namespace lic
{

std::vector<std::string> parse_json_object(const nlohmann::json& msg) 
{
    std::vector<std::string> result;

    if (msg.is_array()) 
    {
        result = msg.get<std::vector<std::string>>();
    }
    else 
    {
        LOG_ERROR("The devices info is not an array in json");
    }

    return result;
}

void signal_handler(int signum) 
{
    LOG_ERROR("Rcv signal: %d", signum);
    save_to_file(LicenseRepo::get_instance().instances(), EnvParser::get_data_path());
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

nlohmann::json parse_license_info()
{
    auto license_info = get_license_info(EnvParser::get_license_path());
    if (license_info.empty())
    {
        std::cerr << "License Error, please make license valid\n";
        std::exit(1);
    }
    
    auto info = nlohmann::json::parse(license_info, nullptr, false);
    if (info.is_discarded())
    {
        std::cerr << "License Content Error, please make license valid\n";
        std::exit(1);
    }

    return info;
}

void init()
{
    init_log();

    auto info = parse_license_info();

    auto issue_date = info["issue_date"];
    auto expire_date = info["expire_date"];
    int max_instance = info["max_instance"];

    auto& inst = LicenseRepo::get_instance();
    inst.set_license_period(LicensePeriod(issue_date, expire_date));
    inst.set_max_instance(max_instance);
    
    auto all_devices = parse_json_object(info["devices"]);
    for (auto& device_id : all_devices)
    {
        inst.add_device(device_id);
    }

    inst.dump();
    InstanceInfos instance_infos{};
    if (load_from_file(EnvParser::get_data_path(), instance_infos))
    {
        inst.recover_devices(instance_infos);
    }
    inst.dump();

    reg_signal();
}

} // namespace lic
