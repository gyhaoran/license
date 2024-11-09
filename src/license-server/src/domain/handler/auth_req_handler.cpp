#include "domain/handler/auth_req_handler.h"
#include "domain/repo/license_repo.h"
#include "domain/msg/auth_req_msg.h"
#include "infra/log/log.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

namespace lic
{

namespace
{

AuthReqMsg parse_auth_req(const json& msg) 
{
    AuthReqMsg info;

    if (msg.is_object()) 
    {
        if (msg.contains("CPU ID") && msg["CPU ID"].is_string()) 
        {
            info.cpu_id = msg["CPU ID"].get<std::string>();
        }

        if (msg.contains("MAC") && msg["MAC"].is_array()) 
        {
            for (const auto& mac : msg["MAC"]) 
            {
                if (mac.is_string())
                {
                    info.mac_addresses.push_back(mac.get<std::string>());
                }
            }
        }
    }

    return info;
}

} // namespace

bool AuthReqHandler::handle(Event& event, const nlohmann::json& msg)
{
    auto req = parse_auth_req(msg);
    auto& cpu_id = req.cpu_id;

    json rsp;
    if (LicenseRepo::get_instance().validate(req, rsp))
    {
        event.update_rsp_msg(rsp.dump());
        return true;
    }
    event.update_rsp_msg(rsp.dump());
    return false;
}

} // namespace lic
