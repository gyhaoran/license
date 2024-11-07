#ifndef D165C218_C7B9_4DD6_9C9D_F86A470C8226
#define D165C218_C7B9_4DD6_9C9D_F86A470C8226

#include <string>

namespace lic
{

struct BaseMsgHandler
{
    virtual bool handle(const std::string& msg) = 0;
};

} // namespace lic



#endif /* D165C218_C7B9_4DD6_9C9D_F86A470C8226 */
