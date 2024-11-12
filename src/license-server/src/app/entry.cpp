#include "app/entry.h"
#include "app/config/env_parser.h"
#include "app/init.h"
#include "service/license_server.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"
#include "infra/log/log.h"

namespace lic
{

void init_entry()
{
    try
    {
        init();
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("init error: %s", e.what());
    } 
}

void main_entry(int argc, char** argv)
{
    init_entry();
    
    SerializationService service(EnvParser::get_data_path());
    service.run();
   
    KeepAliveService alive_service{60};
    alive_service.run();

    LicenseServer server;
    server.run();
}

} // namespace lic
