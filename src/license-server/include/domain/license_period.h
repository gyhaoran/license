#ifndef F642DA11_0D51_4DAC_BAA8_122FFB9EBFFC
#define F642DA11_0D51_4DAC_BAA8_122FFB9EBFFC

#include <string>

namespace lic
{

struct LicensePeriod 
{
    LicensePeriod(const std::string& issue_date="", const std::string& expire_date="");
    bool is_valid() const;

private:
    time_t parse_date(const std::string& date_str) const;

private:
    time_t issue_date_;
    time_t expire_date_;
};

} // namespace lic


#endif /* F642DA11_0D51_4DAC_BAA8_122FFB9EBFFC */
