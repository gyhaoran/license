#ifndef A8107835_A3EE_43F3_A119_0AE9DCD2DAA7
#define A8107835_A3EE_43F3_A119_0AE9DCD2DAA7

#include "domain/handler/json_msg_handler.h"

namespace lic
{

struct AuthReqHandler : JsonMsgHandler
{
private:
    bool handle(const nlohmann::json& msg) override;
};


} // namespace lic

#endif /* A8107835_A3EE_43F3_A119_0AE9DCD2DAA7 */
