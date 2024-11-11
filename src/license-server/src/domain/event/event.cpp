#include "domain/event/event.h"

namespace lic
{

Event::Event(EventId id, const std::string& msg): event_id_{id}, msg_{msg}
{
}

EventId Event::event_id() const
{
    return event_id_;
}

const std::string& Event::get_msg() const
{
    return msg_;
}

std::string Event::get_rsp_msg() const
{
    return rsp_msg_;
}

void Event::update_rsp_msg(const std::string& msg)
{
    rsp_msg_ = msg;
}

} // namespace lic
