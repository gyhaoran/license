#ifndef C044B3E2_719B_4AE9_9F90_8DB6E8D5D007
#define C044B3E2_719B_4AE9_9F90_8DB6E8D5D007

#include "domain/repo/license_repo.h"
#include <string>

namespace lic
{

struct SerializationService
{
    explicit SerializationService(const std::string& file);
    
    void run();
private:
    std::string save_file_;
};

void save_to_file(const LicenseRepo& obj, const std::string& filename);
bool load_from_file(const std::string& filename, DeviceInfos& devices);

} // namespace lic

#endif /* C044B3E2_719B_4AE9_9F90_8DB6E8D5D007 */
