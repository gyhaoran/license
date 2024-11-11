#ifndef C67DAAAE_A64D_4FAA_A946_5D435F347CFB
#define C67DAAAE_A64D_4FAA_A946_5D435F347CFB

#include "domain/handler/json_msg_handler.h"

namespace lic
{

struct InstanceEchoHandler : JsonMsgHandler
{
private:
    bool handle(Event& event, const nlohmann::json& msg) override;
};


} // namespace lic

#endif /* C67DAAAE_A64D_4FAA_A946_5D435F347CFB */
