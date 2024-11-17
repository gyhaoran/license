#include "infra/log/log.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>


namespace lic
{

namespace
{

auto spd_logger = spdlog::rotating_logger_mt("License-Server", "/tmp/.iCell/logs/lic-server.log", 1048576*5, 1);


inline void file_log(LogLevel level, const char* msg)
{
    switch (level)
    {
        case LogLevel::DEBUG:
        {
            spd_logger->debug(msg);
            break;        
        }
        case LogLevel::INFO:
        case LogLevel::SUCC:
        {
            spd_logger->info(msg);
            break;
        }
        case LogLevel::WARN:
        {
            spd_logger->warn(msg);
            break;        
        }
        case LogLevel::ERROR:
        {
            spd_logger->error(msg);
            break;        
        }
        case LogLevel::FATAL:
        {
            spd_logger->critical(msg);
            break;        
        }
        default:
        {
            spd_logger->trace(msg);
            break;
        }
    }
}

static bool g_log_enabled = false;

inline void console_print(LogLevel level, const char* msg)
{
    if (g_log_enabled)
    {
        std::cout << color_fmt_of(level) << std::string("[") + to_string(level) + "]: " 
                << msg << color_fmt_of(LogLevel::NONE) << std::endl;
    }
}

}

void set_log_switch(bool on)
{
    g_log_enabled = on;
}

void infra_print(LogLevel level, const char* msg)
{
    file_log(level, msg);
    console_print(level, msg);
}

} // namespace lic
