#include "fixture/base_fixture.h"
#include "data/constances.h"
#include "app/config/env_parser.h"
#include "app/init.h"
#include "domain/event/event.h"
#include "domain/handler/handle_event.h"
#include "domain/repo/license_repo.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"
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
        LicenseRepo::get_instance().clear_instances();

        LicenseRepo::get_instance().add_device(device_id_1);
        LicenseRepo::get_instance().add_device(device_id_2);
        
    }

    void teardown() override
    {
        LicenseRepo::get_instance().clear_instances();
        for (auto& id : history_)
        {
            LicenseRepo::get_instance().add_device(id);
        }
    }

    void mock_power_on()
    {
        lic::init();
    }

    void mock_power_off()
    {
        LicenseRepo::get_instance().clear();
    }

    std::string send_inst_echo_msg(const InstanceId& inst_id)
    {
        json inst_echo_msg = {
            {"uuid", inst_id}
        };

        Event event{EV_INSTANCE_ECHO, inst_echo_msg.dump()};
        handle_event(event);
        return event.get_rsp_msg();
    }

    std::string send_instance_rel_msg(const InstanceId& inst_id)
    {
        json inst_rel_msg = {
            {"uuid", inst_id}
        };

        Event event{EV_INSTANCE_REL, inst_rel_msg.dump()};
        handle_event(event);
        return event.get_rsp_msg();
    }

    std::string send_auth_req_msg(const InstanceId& inst_id, const std::string& cpuid=VALID_CPU_ID_1, const std::string& mac_id=VALID_MAC_ID_1)
    {
        json req = {
            {"uuid", inst_id},
            {"cpuid", cpuid},
            {"mac", {mac_id}},
        };

        Event event{EV_AUTHRIZATION_REQ, req.dump()};
        handle_event(event);
        return event.get_rsp_msg();
    }

    void device_1_send_auth_req_msg(const InstanceId& inst_id, bool success=true, const std::string& message="")
    {
        auto msg = send_auth_req_msg(inst_id, VALID_CPU_ID_1, VALID_MAC_ID_1);
        auto rsp = json::parse(msg);

        if (success)
        {
            EXPECT_EQ(rsp["status"], "Success");
            EXPECT_EQ(rsp["uuid"], inst_id);
        }
        else
        {
            EXPECT_EQ(rsp["status"], "Failure");
            EXPECT_EQ(rsp["message"], message);
        }
    }

    void device_2_send_auth_req_msg(const InstanceId& inst_id)
    {
        auto msg = send_auth_req_msg(inst_id, VALID_CPU_ID_2, VALID_MAC_ID_2);
        auto rsp = json::parse(msg);

        EXPECT_EQ(rsp["status"], "Success");
        EXPECT_EQ(rsp["uuid"], inst_id);
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

TEST_F(LicenseServerTest, device_1_send_auth_req_rcv_success)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
}

TEST_F(LicenseServerTest, no_auth_device_rcv_auth_failure)
{
    auto msg = send_auth_req_msg(INSTANCE_ID_1, IN_VALID_CPU_ID, IN_VALID_MAC_ID);
    auto rsp = json::parse(msg);
    EXPECT_EQ(rsp["status"], "Failure");
    EXPECT_EQ(rsp["message"], "Device not authorized");
}

TEST_F(LicenseServerTest, device_2_send_auth_req_rcv_success)
{
    device_2_send_auth_req_msg(INSTANCE_ID_2);
}

TEST_F(LicenseServerTest, device_2_send_2_times_auth_req_rcv_success)
{
    device_2_send_auth_req_msg(INSTANCE_ID_2);
    device_2_send_auth_req_msg(INSTANCE_ID_3);
}

TEST_F(LicenseServerTest, rcv_device_auth_req_reach_max_inst)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1, true);
    device_1_send_auth_req_msg(INSTANCE_ID_2, true);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");
}

TEST_F(LicenseServerTest, rcv_device_auth_req_license_expired)
{
    LicensePeriod expired_time_1{"20240101 000000", "20241101 000000"};
    LicenseRepo::get_instance().set_license_period(expired_time_1);
    device_1_send_auth_req_msg(INSTANCE_ID_1, false, "License has expired");

    LicensePeriod expired_time_2{"20250101 000000", "20251101 000000"};
    LicenseRepo::get_instance().set_license_period(expired_time_2);
    device_1_send_auth_req_msg(INSTANCE_ID_1, false, "License has expired");

    LicensePeriod valid_time{"20240101 000000", "20300101 000000"};
    LicenseRepo::get_instance().set_license_period(valid_time);
    device_1_send_auth_req_msg(INSTANCE_ID_1);
}

TEST_F(LicenseServerTest, after_old_inst_release_then_new_inst_auth_success)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_2);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");
    send_instance_rel_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_3);
}

TEST_F(LicenseServerTest, multi_device_multi_inst_auth_success)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_2_send_auth_req_msg(INSTANCE_ID_2);
}

TEST_F(LicenseServerTest, alive_service_remove_timeout_inst_success)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_2);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");

    int inactive_time = 1;
    KeepAliveService service(inactive_time);
    service.run();
    std::this_thread::sleep_for(std::chrono::seconds(3*inactive_time));

    device_1_send_auth_req_msg(INSTANCE_ID_3);
}

TEST_F(LicenseServerTest, client_echo_msg_send_normal_inst_should_keep_alive)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_2);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");

    int inactive_time = 3;
    KeepAliveService service(inactive_time);
    service.run();

    for (int i = 0; i < 2*inactive_time; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send_inst_echo_msg(INSTANCE_ID_1);
        send_inst_echo_msg(INSTANCE_ID_2);
        device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");
    }
}

TEST_F(LicenseServerTest, client_send_echo_msg_and_server_no_this_inst_should_update_inst_info)
{
    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_2);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");

    send_instance_rel_msg(INSTANCE_ID_1);

    send_inst_echo_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_1, false, "Max instance limit reached");
}

TEST_F(LicenseServerTest, server_reboot_should_recover_history_info)
{
    int save_period = 1;
    SerializationService service(EnvParser::get_data_path(), save_period);
    service.run();

    device_1_send_auth_req_msg(INSTANCE_ID_1);
    device_1_send_auth_req_msg(INSTANCE_ID_2);
    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");

    std::this_thread::sleep_for(std::chrono::seconds(2*save_period));

    service.stop();
    mock_power_off();
    mock_power_on();

    device_1_send_auth_req_msg(INSTANCE_ID_3, false, "Max instance limit reached");
}

} // namespace lic_ft
