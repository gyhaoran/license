#ifndef F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66
#define F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66

#include "domain/entities/instance_info.h"
#include <map>

namespace lic
{

using InstanceInfos = std::map<InstanceId, InstanceInfo>;
using DeviceId = std::string;
struct DeviceInfo
{
    DeviceId device_id{""};
    InstanceInfos instances{};

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "device_id: " << device_id << ", current instance num: " << instances.size() << "\n";

        oss << "InstanceInfos: [";
        for (auto& [id, inst] : instances)
        {
            oss << inst.to_string() << ", ";
        }
        oss << "]";

        return oss.str();
    }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("device_id", device_id);
        ar & boost::serialization::make_nvp("instances", instances);
    }
};

} // namespace lic


#endif /* F0C959E2_3C62_4B5F_A7A3_9BD80FA17F66 */
