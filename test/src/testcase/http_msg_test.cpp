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
        server_.run(wait);
    }

    ~HttpMsgTest()
    {
        server_.stop();
    }

    void setup() override
    {
        LicenseRepo::get_instance().clear_device();

        LicenseRepo::get_instance().add_device(device_id_1, DeviceInfo{device_id_1, 1});
        LicenseRepo::get_instance().add_device(device_id_2, DeviceInfo{device_id_2, 2});
    }

    void teardown() override
    {
        LicenseRepo::get_instance().clear_device();
        for (auto& [id, value] : history_)
        {
            LicenseRepo::get_instance().add_device(id, value);
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

    std::string send_inst_echo_msg(const DeviceId& device_id, const InstanceId& inst_id)
    {
        json inst_rel_msg = {
            {"deviceid", device_id},
            {"uuid", inst_id}
        };

        Event event{EV_INSTANCE_ECHO, inst_rel_msg.dump()};
        handle_event(event);
        return event.get_rsp_msg();
    }

    std::string send_instance_rel_msg(const DeviceId& device_id, const InstanceId& inst_id)
    {
        json inst_rel_msg = {
            {"deviceid", device_id},
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

        auto msg = encrypt_info(req.dump());


        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto resp = requests::post("http://127.0.0.1:8442/auth/req", msg, headers);

        Event event{EV_AUTHRIZATION_REQ, req.dump()};
        handle_event(event);
        return event.get_rsp_msg();
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
    send_auth_req_msg(INSTANCE_ID_1);
}

} // namespace lic_ft
