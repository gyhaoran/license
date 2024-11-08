#include "domain/handler/json_msg_handler.h"

namespace lic
{

bool JsonMsgHandler::handle(Event& event)
{
    nlohmann::json req = nlohmann::json::parse(event.get_msg());

    return handle(event, req);
}

} // namespace lic
