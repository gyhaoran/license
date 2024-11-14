#include "initial/power_on.h"
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::lic_ft::power_on();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
