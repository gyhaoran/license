#ifndef H9A128167_02DC_4364_8233_BD4232633E98
#define H9A128167_02DC_4364_8233_BD4232633E98

#include "infra/log/base_name.h"
#include "infra/log/log_level.h"
#include <type_traits>
#include <iostream>

namespace lic
{

void set_log_switch(bool on);
void infra_print(LogLevel level, const char* msg);

namespace detail 
{

template<typename T>
auto printable_cast(T && t) -> decltype(auto) 
{
    if constexpr (std::is_same_v<std::string, std::decay_t<T>>) 
    {
        return std::forward<T>(t).c_str();
    } 
    else 
    {
        return std::forward<T>(t);
    }
}

template<typename ... ARGS>
std::string log_format(const std::string& format, ARGS && ... args)
{
    size_t size = 1 + snprintf(nullptr, 0, format.c_str(), printable_cast(args) ...);
    char bytes[size];
    snprintf(bytes, size, format.c_str(), printable_cast(args) ...);
    return std::string(bytes);
}

template<std::size_t Len, typename ... TS>
void log_print(LogLevel level, const char(&file)[Len], unsigned int line, const char* fmt, TS && ...ts) 
{
    auto msg = std::string(log_basename(file)) + ":" + std::to_string(line) + ": ";
    if constexpr (sizeof...(TS) > 0) 
    {
        msg += log_format(fmt, std::forward<TS>(ts)...);
    } 
    else if constexpr (sizeof...(TS) == 0) 
    {
        msg += fmt;
    }
    infra_print(level, msg.c_str());
}

} // namespace detail

} // namespace lic

#define LOG_PRINT(level, fmt, ...)                         \
do {                                                    \
    if (::lic::level_enabled(level)) {                \
        ::lic::detail::log_print(level,  __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    }                                                    \
} while(0)

#define LOG_FATAL(fmt, ...) LOG_PRINT(::lic::LogLevel::FATAL, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_PRINT(::lic::LogLevel::ERROR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_PRINT(::lic::LogLevel::WARN,  fmt, ##__VA_ARGS__)
#define LOG_SUCC(fmt, ...)  LOG_PRINT(::lic::LogLevel::SUCC,  fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_PRINT(::lic::LogLevel::INFO,  fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_PRINT(::lic::LogLevel::DEBUG, fmt, ##__VA_ARGS__)

#endif
