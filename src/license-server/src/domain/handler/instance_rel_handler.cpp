#include "domain/handler/instance_rel_handler.h"
#include "domain/repo/license_repo.h"

namespace lic
{

using json = nlohmann::json;

DeviceId get_deviceid(const nlohmann::json& msg) 
{
    if (msg.contains("deviceid") && msg["deviceid"].is_string()) 
    {
        return msg["deviceid"].get<std::string>();
    }

    return "";
}

bool InstanceRelHandler::handle(Event& event, const nlohmann::json& msg)
{
    auto device_id = get_deviceid(msg);
    
    LicenseRepo::get_instance().release_inst(device_id);
    json rsp = {{"status", "Success"}};
    event.update_rsp_msg(rsp.dump());
    return true;
}

} // namespace lic
