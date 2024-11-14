#include "fixture/base_fixture.h"
#include "data/constances.h"
#include "domain/event/event.h"
#include "domain/handler/handle_event.h"
#include "domain/repo/license_repo.h"
#include "infra/log/log.h"
#include "device.h"

#include <vector>

using namespace lic;
using json = nlohmann::json;

namespace lic_ft
{

struct LicenseServerTest : BaseFixture
{
    LicenseServerTest() 
        : BaseFixture(), 
        device_id_1{gen_device_hash(VALID_CPU_ID_1, VALID_MAC_ID_1)},
        device_id_2{gen_device_hash(VALID_CPU_ID_2, VALID_MAC_ID_2)},
        history_{LicenseRepo::get_instance().devices()}
    {
    }

    void setup() override
    {
        LicenseRepo::get_instance().clear_device();

        LicenseRepo::get_instance().add_device(device_id_1, DeviceInfo{device_id_1, 1});
        LicenseRepo::get_instance().add_device(device_id_2, DeviceInfo{device_id_2, 1});
        
    }

    void teardown() override
    {
        LicenseRepo::get_instance().clear_device();
        for (auto& [id, value] : history_)
        {
            LicenseRepo::get_instance().add_device(id, value);
        }
    }

protected:
    DeviceId device_id_1;
    DeviceId device_id_2;
    DeviceId invalid_device{"invalia device id"};

private:
    DeviceInfos history_;
};

TEST_F(LicenseServerTest, init)
{
}

TEST_F(LicenseServerTest, rcv_device_1_auth_req)
{
    json req = {
        {"uuid", INSTANCE_ID_1},
        {"cpuid", VALID_CPU_ID_1},
        {"mac", {VALID_MAC_ID_1}},
    };

    Event event{EV_AUTHRIZATION_REQ, req.dump()};
    EXPECT_TRUE(handle_event(event));

    auto rsp = json::parse(event.get_rsp_msg());
    EXPECT_EQ(rsp["status"], "Success");
    EXPECT_EQ(rsp["deviceid"], device_id_1);
}

TEST_F(LicenseServerTest, rcv_device_2_auth_req)
{
    json req = {
        {"uuid", INSTANCE_ID_2},
        {"cpuid", VALID_CPU_ID_2},
        {"mac", {VALID_MAC_ID_2}},
    };

    Event event{EV_AUTHRIZATION_REQ, req.dump()};
    EXPECT_TRUE(handle_event(event));

    auto rsp = json::parse(event.get_rsp_msg());
    EXPECT_EQ(rsp["status"], "Success");
    EXPECT_EQ(rsp["deviceid"], device_id_2);
}

TEST_F(LicenseServerTest, rcv_device_auth_req_reach_max_inst)
{
    json req = {
        {"uuid", INSTANCE_ID_1},
        {"cpuid", VALID_CPU_ID_1},
        {"mac", {VALID_MAC_ID_1}},
    };

    Event event{EV_AUTHRIZATION_REQ, req.dump()};
    EXPECT_TRUE(handle_event(event));

    auto rsp = json::parse(event.get_rsp_msg());
    EXPECT_EQ(rsp["status"], "Success");
    EXPECT_EQ(rsp["deviceid"], device_id_1);

    
    EXPECT_FALSE(handle_event(event));

    rsp = json::parse(event.get_rsp_msg());
    EXPECT_EQ(rsp["status"], "Failure");
    EXPECT_EQ(rsp["message"], "Max instance limit reached");
}

} // namespace lic_ft
