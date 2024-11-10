#ifndef B785EFB0_23C0_4E3F_8791_1D97C8749998
#define B785EFB0_23C0_4E3F_8791_1D97C8749998

#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include <sstream>
#include <chrono>

namespace lic
{

using InstanceId = std::string;
struct InstanceInfo
{
    InstanceId instance_id;
    std::chrono::steady_clock::time_point last_heartbeat;

    std::string to_string() const
    {
        std::ostringstream oss;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(last_heartbeat.time_since_epoch());
        oss << "instance_id: " << instance_id << ", last_heartbeat: " << duration.count();
        return oss.str();
    }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) 
    {
        ar & boost::serialization::make_nvp("instance_id", instance_id);

        auto time_since_epoch = last_heartbeat.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
        ar & boost::serialization::make_nvp("last_heartbeat", seconds);

        if (Archive::is_loading::value) 
        {
            last_heartbeat = std::chrono::steady_clock::time_point(std::chrono::seconds(seconds));
        }
    }
};

} // namespace lic

#endif /* B785EFB0_23C0_4E3F_8791_1D97C8749998 */
