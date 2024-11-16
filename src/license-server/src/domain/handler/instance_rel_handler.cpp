#include "domain/handler/instance_rel_handler.h"
#include "domain/msg/inst_rel_msg.h"
#include "domain/repo/license_repo.h"

using json = nlohmann::json;

namespace lic
{

namespace
{

InstRelMsg parse_inst_rel_msg(const nlohmann::json& msg) 
{
    InstRelMsg result;
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

bool InstanceRelHandler::handle(Event& event, const nlohmann::json& msg)
{
    auto rel_msg = parse_inst_rel_msg(msg);
    
    LicenseRepo::get_instance().release_instance(rel_msg.instance_id);

    return true;
}

} // namespace lic
