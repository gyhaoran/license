#include "license_info.h"
#include "json.hpp"

using json = nlohmann::json;

namespace lic
{

LicenseInfo parse_license_info(const std::string& license_info_json) 
{
    LicenseInfo license_info;
    auto json_data = json::parse(license_info_json.c_str());

    license_info.license_id = json_data["id"].get<std::string>();
    license_info.app_hash = json_data["app_info"].get<std::string>();
    license_info.max_instance = json_data["max_instance"].get<int>();
    license_info.is_server = json_data["is_server"].get<bool>();
    license_info.ether_name = json_data["ether_name"].get<std::string>();
    license_info.device_hash = json_data["device_info"].get<std::string>();
    license_info.issue_date = json_data["issue_date"].get<std::string>();
    license_info.expire_date = json_data["expiration"].get<std::string>();

    return license_info;
}

} // namespace lic
