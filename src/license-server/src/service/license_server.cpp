#include "service/license_server.h"
#include "app/config/env_parser.h"
#include "domain/event/event.h"
#include "domain/handler/handle_event.h"
#include "infra/log/log.h"
#include "utils.h"
#include <hv/HttpServer.h>
#include <hv/HttpResponseWriter.h>
#include <hv/hasync.h>

using json = nlohmann::json;

namespace lic
{

namespace
{

std::string handle_http_msg(EventId id, const std::string& msg)
{
    Event event(id, msg);
    if (!handle_event(event))
    {
        LOG_WARN("EventId %u handle failed", id);
    }
    return event.get_rsp_msg();
}

void send_http_rsp(const HttpResponseWriterPtr& writer, const std::string& rsp)
{
    LOG_INFO("send_http_rsp msg: %s", rsp.c_str());
    auto encode_msg = encrypt_info(rsp);
    
    writer->WriteHeader("Content-Type", "application/octet-stream");
    writer->End(encode_msg);
}

void handle_authen_req(const std::string& msg, const HttpResponseWriterPtr& writer)
{
    try 
    {
        LOG_INFO("Rcv EV_AUTHRIZATION_REQ origin msg: %s", msg.c_str());
        auto decode_msg = decrypt_info(msg);
        LOG_INFO("Rcv EV_AUTHRIZATION_REQ decode_msg: %s", decode_msg.c_str());
        
        auto rsp = handle_http_msg(EV_AUTHRIZATION_REQ, decode_msg);
        send_http_rsp(writer, rsp);
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR("Handle EV_AUTHRIZATION_REQ error: %s", e.what());
        writer->WriteStatus(HTTP_STATUS_BAD_REQUEST);
        writer->End(R"({"status": "Failure", "message": "unkonwn"})");
    }
    catch (...)
    {
        LOG_ERROR("Handle EV_AUTHRIZATION_REQ error:");
        writer->WriteStatus(HTTP_STATUS_BAD_REQUEST);
        writer->End(R"({"status": "Failure", "message": "unkonwn"})");
    }
}

void handle_inst_release(const std::string& msg, const HttpResponseWriterPtr& writer)
{
    try 
    {
        LOG_INFO("Rcv EV_INSTANCE_REL msg: %s", msg.c_str());
        auto rsp = handle_http_msg(EV_INSTANCE_REL, msg);
        writer->End();
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR("Handle EV_INSTANCE_REL error: %s", e.what());
        writer->End();
    }
}

void handle_inst_echo(const std::string& msg, const HttpResponseWriterPtr& writer)
{
    try 
    {
        LOG_INFO("Rcv EV_INSTANCE_ECHO msg: %s", msg.c_str());
        writer->End();
        auto rsp = handle_http_msg(EV_INSTANCE_ECHO, msg);
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR("Handle EV_INSTANCE_ECHO error: %s", e.what());
        writer->End();
    }
}

} // namespace

LicenseServer::LicenseServer(int port) : port_{port}, service_{new ::hv::HttpService()} 
{
    service_->POST("/auth/license",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        std::cout << req->GetHeader("Content-Type") << '\n';
        handle_authen_req(req->body, writer);
    });

    service_->POST("/inst/rel",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_inst_release(req->body, writer);
    });

    service_->POST("/inst/echo", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_inst_echo(req->body, writer);
    });
}

LicenseServer::~LicenseServer()
{
    delete service_;
    service_ = nullptr;
}

void LicenseServer::run()
{
    static hv::HttpServer server;
    server.service = service_;
    server.port = port_;

    server.run();
    ::hv::async::cleanup();
    // std::atexit(::hv::async::cleanup);
}

} // namespace lic
