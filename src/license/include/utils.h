#ifndef BBCC975A_C556_4B7F_AF68_9C101C714496
#define BBCC975A_C556_4B7F_AF68_9C101C714496

#include <string>

namespace lic
{

std::string decrypt_info(const std::string& data, const std::string& key="");    
std::string encrypt_info(const std::string& data, const std::string& key="");

} // namespace lic

#endif /* BBCC975A_C556_4B7F_AF68_9C101C714496 */
