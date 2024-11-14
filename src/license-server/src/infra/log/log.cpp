#include "infra/log/log.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>


namespace lic
{

namespace
{

auto spd_logger = spdlog::rotating_logger_mt("License-Server", "/tmp/.iCell/logs/lic-server.log", 1048576*5, 1);

}

void infra_print(LogLevel level, const char* msg)
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

} // namespace lic
