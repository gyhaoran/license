#ifndef A4DF954B_B6A2_4B97_8E10_4339200E83BA
#define A4DF954B_B6A2_4B97_8E10_4339200E83BA

#include <gtest/gtest.h>

namespace lic_ft
{

struct BaseFixture : ::testing::Test
{
    void SetUp() override;
    void TearDown() override;

protected:
    virtual void setup();
    virtual void teardown();
};

} // namespace lic_ft

#endif /* A4DF954B_B6A2_4B97_8E10_4339200E83BA */
