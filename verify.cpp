#include "activator.h"
#include <iostream>

using namespace lic;

int main(int argc, char** argv)
{
    if (!verify("./license.dat"))
    {
        std::cerr << "License Activate failed\n";
    }

    std::cout << "License Activate success\n";
    return 0;
}
