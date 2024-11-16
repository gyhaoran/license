#include "domain/handler/instance_echo_handler.h"
#include "domain/msg/inst_echo_msg.h"
#include "domain/repo/license_repo.h"
#include "infra/log/log.h"

using json = nlohmann::json;

namespace lic
{

namespace
{

InstEchoMsg parse_inst_echo_msg(const nlohmann::json& msg) 
{
    InstEchoMsg result;
    if (msg.is_object()) 
    {
        if (!msg.contains("uuid"))
        {
            return result;
        }
        
        if (msg["uuid"].is_string())
        {
            result.instance_id = msg["uuid"].get<std::string>();
        }
    }

    return result;
}

} // namespace

bool InstanceEchoHandler::handle(Event& event, const nlohmann::json& msg)
{
    auto echo_msg = parse_inst_echo_msg(msg);
    
    LicenseRepo::get_instance().update_instance(echo_msg.instance_id);

    return true;
}

} // namespace lic
