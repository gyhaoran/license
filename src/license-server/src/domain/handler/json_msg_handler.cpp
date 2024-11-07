#include "domain/handler/json_msg_handler.h"

namespace lic
{

bool JsonMsgHandler::handle(const std::string& msg)
{
    nlohmann::json req = nlohmann::json::parse(msg);

    return handle(req);
}

} // namespace lic
