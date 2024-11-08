#ifndef F6E2D024_9CB7_4552_AC23_27937F1AEBEC
#define F6E2D024_9CB7_4552_AC23_27937F1AEBEC

#include <string>

namespace lic
{
    
std::string gen_device_hash(const std::string& cpu_id, const std::string& mac_addr);

std::string get_device_hash(bool is_server, const std::string& mac_addr);

} // namespace lic

#endif /* F6E2D024_9CB7_4552_AC23_27937F1AEBEC */
