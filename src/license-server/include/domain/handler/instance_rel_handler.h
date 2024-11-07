#ifndef F84C1A32_C7C5_422D_8125_2734373E0705
#define F84C1A32_C7C5_422D_8125_2734373E0705

#include "domain/handler/json_msg_handler.h"

namespace lic
{

struct InstanceRelHandler : JsonMsgHandler
{
private:
    bool handle(const nlohmann::json& msg) override;
};


} // namespace lic


#endif /* F84C1A32_C7C5_422D_8125_2734373E0705 */
