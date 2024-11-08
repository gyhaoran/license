#ifndef DD292DE7_82ED_4622_AF41_C054623C04DE
#define DD292DE7_82ED_4622_AF41_C054623C04DE

#include "domain/event/event_id.h"
#include <string>

namespace lic
{

struct Event
{
    Event(EventId id, const std::string& msg);

    const std::string& get_msg() const;
    std::string get_rsp_msg() const;
    
    void update_rsp_msg(const std::string& msg);

private:
    EventId event_id_{EV_INVALID_ID};

    std::string msg_{""};
    std::string rsp_msg_{""};
};

} // namespace lic

#endif /* DD292DE7_82ED_4622_AF41_C054623C04DE */
