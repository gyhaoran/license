#ifndef CBD60B14_1621_4194_B425_F8A875A2BCFE
#define CBD60B14_1621_4194_B425_F8A875A2BCFE

#include <string>
#include <vector>

namespace lic
{
    
struct AuthReqMsg 
{
    std::string cpu_id{""};
    std::vector<std::string> mac_addresses{};
};

} // namespace lic


#endif /* CBD60B14_1621_4194_B425_F8A875A2BCFE */
