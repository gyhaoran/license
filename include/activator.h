#ifndef FCA83E9C_25B0_47B8_9BAE_63E95A11BD8D
#define FCA83E9C_25B0_47B8_9BAE_63E95A11BD8D

#include <string>

namespace lic
{

struct LicenseHeader;
struct LicenseData;

bool validate_license(const LicenseHeader& header, const LicenseData& data);

bool verify(const std::string& license_file);

} // namespace lic

#endif /* FCA83E9C_25B0_47B8_9BAE_63E95A11BD8D */
