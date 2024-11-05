#ifndef CF9B49FA_ACE0_445E_B56C_B960DDFF1353
#define CF9B49FA_ACE0_445E_B56C_B960DDFF1353

#include <string>

namespace lic
{

struct LicenseInfo 
{
    std::string license_id;
    std::string app_hash;
    std::string max_instance;
    bool is_server;                
    std::string ether_name;
    std::string device_hash;
    std::string issue_date;
    std::string expire_date;
};

LicenseInfo parse_license_info(const std::string& license_info_json);

} // namespace lic

#endif /* CF9B49FA_ACE0_445E_B56C_B960DDFF1353 */
