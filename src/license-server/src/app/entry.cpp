#include "app/entry.h"
#include "app/config/env_parser.h"
#include "app/config/arg_parser.h"
#include "app/init.h"
#include "service/license_server.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"
#include "infra/log/log.h"
#include "infra/sign/signature.h"

namespace lic
{

namespace
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

void verify_license_server_sig()
{
    auto sig_file = EnvParser::get_server_sig_path();
    auto cert_file = EnvParser::get_server_cert_path();
    
    if (!verify_sig(sig_file, cert_file))
    {
        std::cerr << "package sig check failed, exit.\n";
        std::exit(1);
    }
}

} // namespace

void main_entry(int argc, char** argv)
{
    verify_license_server_sig();
    ArgParser::parse_arguments(argc, argv);

    init_entry();
    
    SerializationService service(EnvParser::get_data_path());
    service.run();
   
    KeepAliveService alive_service{60};
    alive_service.run();

    LicenseServer server;
    server.run();
}

} // namespace lic
