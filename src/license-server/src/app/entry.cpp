#include "app/entry.h"
#include "app/arg_parser.h"
#include "app/init.h"
#include "service/license_server.h"

namespace lic
{

void main_entry(int argc, char** argv)
{
    init();

    LicenseServer server;
    server.run();
}
    
} // namespace lic
