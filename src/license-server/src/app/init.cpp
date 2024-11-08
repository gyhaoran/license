#include "app/init.h"
#include "domain/repo/license_repo.h"
#include "domain/license_period.h"
#include "license_parser.h"
#include "hash.h"
#include "json.hpp"
#include <iostream>

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
                std::cerr << "Value for key '" << key << "' is not an integer.\n";
            }
        }
    }
    else 
    {
        std::cerr << "The provided JSON is not an object.\n";
    }

    return result;
}

void init()
{
    auto license_info = get_license_info("./license.dat");
    auto info = nlohmann::json::parse(license_info);

    auto issue_date = info["issue_date"];
    auto expire_date = info["expire_date"];

    auto& inst = LicenseRepo::get_instance();
    inst.set_license_period(LicensePeriod(issue_date, expire_date));

    auto devices = parse_json_object(info["devices"]);
    for (auto& [key, value] : devices)
    {
        inst.add_device(key, DeviceInfo(key, value, 0));
    }
}

} // namespace lic
