#include "activator.h"
#include <iostream>

using namespace lic;

int main(int argc, char** argv)
{
    if (!verify("./license.dat"))
    {
        std::cerr << "Liicense Activate failed\n";
    }

    return 0;
}
