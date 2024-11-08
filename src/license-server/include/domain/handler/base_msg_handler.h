#ifndef D165C218_C7B9_4DD6_9C9D_F86A470C8226
#define D165C218_C7B9_4DD6_9C9D_F86A470C8226

#include <domain/event/event.h>

namespace lic
{

struct BaseMsgHandler
{
    virtual bool handle(Event& event) = 0;
};

} // namespace lic



#endif /* D165C218_C7B9_4DD6_9C9D_F86A470C8226 */
