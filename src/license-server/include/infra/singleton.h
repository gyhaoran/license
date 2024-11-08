#ifndef AAA230A5_31B7_4915_9205_BA69F977D660
#define AAA230A5_31B7_4915_9205_BA69F977D660

#include "infra/uncloneable.h"

namespace lic
{

template<typename T>
struct Singleton
{
    static T& get_instance()
    {
        static T instance;
        return instance;
    }

    DISALLOW_COPY_AND_ASSIGN(Singleton)

protected:
    Singleton() {}
};

#define DEF_SINGLETON(object) struct object : ::lic::Singleton<object>

} // namespace lic


#endif /* AAA230A5_31B7_4915_9205_BA69F977D660 */
