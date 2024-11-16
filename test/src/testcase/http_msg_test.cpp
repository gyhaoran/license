#include "fixture/base_fixture.h"
#include "data/constances.h"
#include "app/config/env_parser.h"
#include "app/init.h"
#include "domain/event/event.h"
#include "domain/handler/handle_event.h"
#include "domain/repo/license_repo.h"
#include "service/keep_alive_service.h"
#include "service/serialization_service.h"
#include "service/license_server.h"
#include "infra/log/log.h"
#include "device.h"
#include "utils.h"
#include <hv/HttpClient.h>
#include <hv/requests.h>
#include <vector>


using namespace lic;
using namespace hv;
using json = nlohmann::json;

namespace lic_ft
{

struct HttpMsgTest : BaseFixture
{
    HttpMsgTest() : BaseFixture(), 
        device_id_1{gen_device_hash(VALID_CPU_ID_1, VALID_MAC_ID_1)},
        device_id_2{gen_device_hash(VALID_CPU_ID_2, VALID_MAC_ID_2)},
        history_{LicenseRepo::get_instance().devices()}
    {
        start();
    }

    ~HttpMsgTest()
    {
        stop();
    }

    void start()
    {
        server_.run(false);
    }

    void stop()
    {
        server_.stop();
    }

    void setup() override
    {
        LicenseRepo::get_instance().clear_instances();
        LicenseRepo::get_instance().set_max_instance(2);
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

    requests::Response send_inst_echo_http_msg(const InstanceId& inst_id)
    {
        json inst_echo_msg = {
            {"uuid", inst_id}
        };

        auto msg = encrypt_info(inst_echo_msg.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto rsp = requests::post("http://127.0.0.1:8442/inst/echo", msg, headers);

        return rsp;
    }

    requests::Response send_instance_rel_http_msg(const InstanceId& inst_id)
    {
        json inst_rel_msg = {
            {"uuid", inst_id}
        };

        auto msg = encrypt_info(inst_rel_msg.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto rsp = requests::post("http://127.0.0.1:8442/inst/rel", msg, headers);

        return rsp;
    }

    requests::Response send_auth_req_http_msg(const json& req)
    {
        auto msg = encrypt_info(req.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";

        auto rsp = requests::post("http://127.0.0.1:8442/auth/license", msg, headers);
        return rsp;
    }

    requests::Response send_auth_req_http_msg(const InstanceId& inst_id, const std::string& cpuid=VALID_CPU_ID_1, const std::string& mac_id=VALID_MAC_ID_1)
    {
        json req = {
            {"uuid", inst_id},
            {"cpuid", cpuid},
            {"mac", {mac_id}},
        };
        return send_auth_req_http_msg(req);
    }

    void check_auth_rsp(const json& rsp, const InstanceId& inst_id, bool success=true, const std::string& message="")
    {
        if (success)
        {
            ASSERT_EQ(rsp["status"], "Success");
            ASSERT_TRUE(rsp.contains("uuid"));
            ASSERT_EQ(rsp["uuid"], inst_id);
        }
        else
        {
            ASSERT_EQ(rsp["status"], "Failure");
            ASSERT_EQ(rsp["message"], message);
        }
    }

protected:
    DeviceId device_id_1;
    DeviceId device_id_2;
    DeviceId invalid_device{"invalia device id"};

private:
    DeviceInfos history_;
    LicenseServer server_{};
};

TEST_F(HttpMsgTest, device_1_send_auth_req_rcv_success)
{
    auto rsp = send_auth_req_http_msg(INSTANCE_ID_1);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_1);
}

TEST_F(HttpMsgTest, rcv_device_auth_req_reach_max_inst)
{
    send_auth_req_http_msg(INSTANCE_ID_1);
    send_auth_req_http_msg(INSTANCE_ID_2);

    auto rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_3, false, "Max instance limit reached");
}

TEST_F(HttpMsgTest, auth_req_and_after_inst_relase_then_can_auth_success)
{
    send_auth_req_http_msg(INSTANCE_ID_1);
    send_auth_req_http_msg(INSTANCE_ID_2);

    auto rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_3, false, "Max instance limit reached");

    send_instance_rel_http_msg(INSTANCE_ID_1);

    rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_3);
}

TEST_F(HttpMsgTest, client_echo_msg_send_normal_inst_should_keep_alive)
{
    /* Device1 instance-1 */
    auto rsp = send_auth_req_http_msg(INSTANCE_ID_1);
    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_1);

    rsp = send_auth_req_http_msg(INSTANCE_ID_2);
    msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_2);

    int inactive_time = 2;
    KeepAliveService service(inactive_time);
    service.run();

    for (int i = 0; i < inactive_time; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        send_inst_echo_http_msg(INSTANCE_ID_1);
        send_inst_echo_http_msg(INSTANCE_ID_2);

        rsp = send_auth_req_http_msg(INSTANCE_ID_3);
        ASSERT_TRUE(rsp);
        EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
        msg = json::parse(decrypt_info(rsp->body));
        check_auth_rsp(msg, INSTANCE_ID_3, false, "Max instance limit reached");
    }

    std::this_thread::sleep_for(std::chrono::seconds(4));

    LOG_INFO("sleep 4s end");
    rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_3);
}

TEST_F(HttpMsgTest, multi_client_send_license_auth_req_http_msg)
{
    /* Device1 instance-1 */
    auto rsp = send_auth_req_http_msg(INSTANCE_ID_1);
    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_1);

    /* Device2 instance-1 */
    rsp = send_auth_req_http_msg(INSTANCE_ID_2, VALID_CPU_ID_2, VALID_MAC_ID_2);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_2);
}

TEST_F(HttpMsgTest, device_1_multi_mac_one_correct_should_success)
{
    json req = {
        {"uuid", INSTANCE_ID_1},
        {"cpuid", VALID_CPU_ID_1},
        {"mac", {IN_VALID_MAC_ID, VALID_MAC_ID_1}},
    };
    auto rsp = send_auth_req_http_msg(req);

    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_1);
}


/***************************** Error Case ******************************/

TEST_F(HttpMsgTest, ErrorCase__auth_req_msg_content_error_no_instance_id)
{
    json req = {
        {"cpuid", VALID_CPU_ID_1},
        {"mac", {IN_VALID_MAC_ID, VALID_MAC_ID_1}},
    };
    auto rsp = send_auth_req_http_msg(req);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, "", false, "Device not authorized");
}

TEST_F(HttpMsgTest, ErrorCase__auth_req_msg_content_error_no_cpu_id)
{
    json req = {
        {"uuid", INSTANCE_ID_1}, 
        {"mac", {IN_VALID_MAC_ID, VALID_MAC_ID_1}},
    };
    auto rsp = send_auth_req_http_msg(req);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, device_id_1, false, "Device not authorized");
}

TEST_F(HttpMsgTest, ErrorCase__auth_req_msg_content_error_no_mac_addr)
{
    json req = {
        {"uuid", INSTANCE_ID_1}, 
        {"cpuid", VALID_CPU_ID_1},
    };
    auto rsp = send_auth_req_http_msg(req);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, device_id_1, false, "Device not authorized");
}

TEST_F(HttpMsgTest, ErrorCase__auth_req_msg_content_format_error)
{
    json req = {
        {"uuid", INSTANCE_ID_1}, 
        {"cpuid", VALID_CPU_ID_1},
        {"mac", VALID_MAC_ID_1},
    };
    auto rsp = send_auth_req_http_msg(req);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, device_id_1, false, "Device not authorized");
}

TEST_F(HttpMsgTest, ErrorCase__auth_req_msg_cannot_reach_server)
{
    stop();
    auto rsp = send_auth_req_http_msg(INSTANCE_ID_1);
    EXPECT_EQ(rsp, nullptr);
}

TEST_F(HttpMsgTest, ErrorCase__inst_release_msg_cannot_reach_server)
{
    auto rsp = send_auth_req_http_msg(INSTANCE_ID_1);
    rsp = send_auth_req_http_msg(INSTANCE_ID_2);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);

    auto msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_2);

    stop();
    rsp = send_instance_rel_http_msg(INSTANCE_ID_1);
    EXPECT_EQ(rsp, nullptr);

    rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    EXPECT_EQ(rsp, nullptr);

    start();
    rsp = send_auth_req_http_msg(INSTANCE_ID_3);
    EXPECT_EQ(rsp->status_code, HTTP_STATUS_OK);
    msg = json::parse(decrypt_info(rsp->body));
    check_auth_rsp(msg, INSTANCE_ID_1, false, "Max instance limit reached");
}

} // namespace lic_ft
