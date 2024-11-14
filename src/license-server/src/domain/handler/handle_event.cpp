#include "domain/handler/handle_event.h"
#include "domain/event/event.h"
#include "domain/handler/auth_req_handler.h"
#include "domain/handler/instance_rel_handler.h"
#include "domain/handler/instance_echo_handler.h"
#include "infra/log/log.h"

#include <functional>
#include <map>

namespace lic
{

namespace
{

std::map<EventId, BaseMsgHandler*> all_handlers = 
{
    {EV_AUTHRIZATION_REQ,     new AuthReqHandler()},
    {EV_INSTANCE_REL,         new InstanceRelHandler()},
    {EV_INSTANCE_ECHO,        new InstanceEchoHandler()},
};

} // namespace

bool handle_event(Event& event)
{
    auto id = event.event_id();
    auto iter = all_handlers.find(id);
    if (iter != all_handlers.end())
    {
        return iter->second->handle(event);
    }

    LOG_ERROR("Unexpected eventid: %u", id);
    return false;
}

} // namespace lic
