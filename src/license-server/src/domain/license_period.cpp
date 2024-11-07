#include "domain/license_period.h"
#include <ctime>

namespace lic
{

LicensePeriod::LicensePeriod(const std::string& start_date, const std::string& end_date) 
: issue_date_{parse_date(start_date)}, expire_date_{parse_date(end_date)} 
{
}

bool LicensePeriod::is_valid() const 
{
    time_t now = time(nullptr);
    return issue_date_ <= now && now <= expire_date_;
}

time_t LicensePeriod::parse_date(const std::string& date_str) const 
{
    struct tm tm;
    strptime(date_str.c_str(), "%Y%m%d %H%M%S", &tm);
    return mktime(&tm);
}

} // namespace lic
