#include "fixture/base_fixture.h"

namespace lic_ft
{

void BaseFixture::SetUp()
{
    ::testing::Test::SetUp();
    setup();
}

void BaseFixture::TearDown()
{
    ::testing::Test::TearDown();
    teardown();
}

void BaseFixture::setup()
{
}

void BaseFixture::teardown()
{
}

} // namespace lic_ft
