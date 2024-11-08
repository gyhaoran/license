#ifndef E6A111B0_F4C6_4A9A_B106_4EFEC5A11457
#define E6A111B0_F4C6_4A9A_B106_4EFEC5A11457

#include "domain/handler/base_msg_handler.h"
#include "json.hpp"

namespace lic
{

struct JsonMsgHandler : BaseMsgHandler
{
private:
    bool handle(Event& event) override;
    virtual bool handle(Event& event, const nlohmann::json& msg) = 0;
};


} // namespace lic


#endif /* E6A111B0_F4C6_4A9A_B106_4EFEC5A11457 */
