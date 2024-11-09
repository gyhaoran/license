#ifndef F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66
#define F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66

#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include <sstream>

namespace lic
{

using DeviceId = std::string;

struct DeviceInfo
{
    std::string hash{""};
    int max_instance{1};
    int current_instance{0};

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "id: " << hash << ", current_instance: " << current_instance 
            << ", max_instance: " << max_instance;
        return oss.str();
    }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("hash", hash);
        ar & boost::serialization::make_nvp("max_instance", max_instance);
        ar & boost::serialization::make_nvp("current_instance", current_instance);
    }
};

} // namespace lic


#endif /* F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66 */
