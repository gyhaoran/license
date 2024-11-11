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
    AuthReqMsg req;

    if (msg.is_object()) 
    {
        if (!msg.contains("uuid") || !msg.contains("cpuid") || !msg.contains("mac"))
        {
            return req;
        }
        
        if (msg["uuid"].is_string())
        {
            req.instance_id = msg["uuid"].get<std::string>();
        }

        if (msg["cpuid"].is_string()) 
        {
            req.cpu_id = msg["cpuid"].get<std::string>();
        }

        if (msg.contains("mac") && msg["mac"].is_array()) 
        {
            for (const auto& mac : msg["mac"]) 
            {
                if (mac.is_string())
                {
                    req.mac_addresses.push_back(mac.get<std::string>());
                }
            }
        }
    }

    return req;
}

} // namespace

bool AuthReqHandler::handle(Event& event, const nlohmann::json& msg)
{
    auto req = parse_auth_req(msg);
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
