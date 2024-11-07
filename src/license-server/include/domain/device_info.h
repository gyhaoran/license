#ifndef F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66
#define F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66

#include <string>

namespace lic
{

using DeviceId = std::string;

struct DeviceInfo
{
    std::string hash{""};
    int max_instance{1};
    int current_instance{0};
};

} // namespace lic


#endif /* F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66 */
