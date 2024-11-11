#include "app/entry.h"
#include "app/arg_parser.h"
#include "app/init.h"
#include "service/license_server.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"

namespace lic
{

void main_entry(int argc, char** argv)
{
    init();
    
    SerializationService service("./info.dat");
    service.run();
   
    KeepAliveService alive_service{60};
    alive_service.run();

    LicenseServer server;
    server.run();
    
    // while (true)
    // {
    // }
}

} // namespace lic
