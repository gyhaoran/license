#ifndef FA7178F3_A64A_4F1C_8271_3BEAEA3B605D
#define FA7178F3_A64A_4F1C_8271_3BEAEA3B605D

#include <string>

namespace lic
{

std::string decrypt_info(const std::string& data, const std::string& key="");    
std::string encrypt_info(const std::string& data, const std::string& key="");

} // namespace lic

#endif /* FA7178F3_A64A_4F1C_8271_3BEAEA3B605D */
