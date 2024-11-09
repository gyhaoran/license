#ifndef CBA0C3BA_CB11_453F_87BB_997F756179B6
#define CBA0C3BA_CB11_453F_87BB_997F756179B6

#include <type_traits>

namespace lic
{

template<typename E>
constexpr auto enum_underlying_cast(E e) noexcept 
{
    return static_cast<std::underlying_type_t<E>>(e);
}

} // namespace lic

#endif /* CBA0C3BA_CB11_453F_87BB_997F756179B6 */
