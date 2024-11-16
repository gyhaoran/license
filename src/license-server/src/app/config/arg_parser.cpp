#include "app/config/arg_parser.h"
#include "argparse.hpp"
#include <iostream>
#include <string>


namespace lic
{

void ArgParser::parse_arguments(int argc, char* argv[]) 
{
    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("-v", "--version")
            .action("version")
            .version("License Server 1.0")
            .help("display the version of the license server");

    parser.add_argument("--log")
            .action("store_true")
            .help("enable logging");

    auto const args = parser.parse_args();

    bool log_enabled = args.get<bool>("log");
    if (log_enabled)
    {
        std::cout << "Logging enabled: " << std::boolalpha << log_enabled << "\n";
    }
}

} // namespace lic

