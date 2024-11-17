#include "verify.h"
#include "encoder.h"
#include <hv/requests.h>
#include "json.hpp"

using InstanceId = std::string;
using json = nlohmann::json;

namespace lic
{

namespace
{

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

requests::Response send_auth_req_http_msg(const std::string& cpuid=VALID_CPU_ID_1, const std::string& mac_id=VALID_MAC_ID_1)
{
    json req = {
        {"cpuid", cpuid},
        {"mac", {mac_id}},
    };
    return send_auth_req_http_msg(req);
}

InstanceId check_auth_rsp(const json& rsp, bool success=true, const std::string& message="")
{
    if (success)
    {
        EXPECT_EQ(rsp["status"], "Success");
        EXPECT_TRUE(rsp.contains("uuid"));
        return rsp["uuid"].get<std::string>();
    }
    else
    {
        EXPECT_EQ(rsp["status"], "Failure");
        EXPECT_EQ(rsp["message"], message);
    }
    return "";
}

} // namespace


bool verify(const std::string& ip, int port=8442)
{
    return true;
}

} // namespace lic
