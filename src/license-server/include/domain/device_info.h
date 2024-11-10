#ifndef F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66
#define F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66

#include "domain/instance_info.h"
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include <sstream>
#include <map>

namespace lic
{

using InstanceInfos = std::map<InstanceId, InstanceInfo>;
using DeviceId = std::string;
struct DeviceInfo
{
    DeviceId device_id{""};
    int max_instance{1};
    int current_instance{0}; // equal instances.size();
    InstanceInfos instances{};

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "id: " << device_id << ", current_instance: " << current_instance 
            << ", max_instance: " << max_instance;
        return oss.str();
    }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("device_id", device_id);
        ar & boost::serialization::make_nvp("max_instance", max_instance);
        ar & boost::serialization::make_nvp("current_instance", current_instance);
    }
};

} // namespace lic


#endif /* F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66 */
