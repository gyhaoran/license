#include "app/entry.h"
#include "app/config/env_parser.h"
#include "app/init.h"
#include "service/license_server.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"

namespace lic
{

void main_entry(int argc, char** argv)
{
    init();
    
    SerializationService service(EnvParser::get_data_path());
    service.run();
   
    KeepAliveService alive_service{60};
    alive_service.run();

    LicenseServer server;
    server.run();
}

} // namespace lic
