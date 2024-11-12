#include "app/config/env_parser.h"
#include "infra/log/log.h"
#include <cstdlib>

namespace lic
{

std::string EnvParser::get_license_path()
{
    const char* data_path = std::getenv("ICELL_LIC_SERVER_DATA");

    if (data_path == nullptr) 
    {   
        LOG_WARN("Fail to get license path");
        return "license.dat";
    }

    return std::string(data_path) + "/license.dat";
}

std::string EnvParser::get_data_path()
{
    const char* data_path = std::getenv("ICELL_LIC_SERVER_DATA");

    if (data_path == nullptr) 
    {
        LOG_WARN("Fail to get data path");
        return "info.dat";
    }

    return std::string(data_path) + "/info.dat";
}

} // namespace lic
