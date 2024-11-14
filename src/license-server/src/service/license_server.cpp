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
    auto decode_msg = decrypt_info(msg);

    Event event(id, decode_msg);
    if (!handle_event(event))
    {
        LOG_WARN("EventId %u handle failed", id);
    }
    return event.get_rsp_msg();
}

void send_http_rsp(const HttpResponseWriterPtr& writer, const std::string& rsp)
{   
    if (rsp.empty())
    {
        writer->End();
        return;
    }

    auto encode_msg = encrypt_info(rsp);    
    writer->WriteHeader("Content-Type", "application/octet-stream");
    writer->End(encode_msg);
}

void send_http_bad_rsp(const HttpResponseWriterPtr& writer)
{
    writer->WriteStatus(HTTP_STATUS_BAD_REQUEST);
    writer->End();
}

void handle_http_req(EventId id, const HttpRequestPtr& req, const HttpResponseWriterPtr& writer)
{
    try 
    {
        auto rsp = handle_http_msg(id, req->body);
        send_http_rsp(writer, rsp);
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR("Handle Event: %s error: %s", ev_id_str(id), e.what());
        send_http_bad_rsp(writer);
    }
    catch (...)
    {
        LOG_ERROR("Handle Event: %s error, errno info: %s", ev_id_str(id), std::strerror(errno));
        send_http_bad_rsp(writer);
    }
}

} // namespace

LicenseServer::LicenseServer(int port) : port_{port}, service_{new ::hv::HttpService()} 
{
    service_->POST("/auth/license",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_http_req(EV_AUTHRIZATION_REQ, req, writer);
    });

    service_->POST("/inst/rel",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_http_req(EV_INSTANCE_REL, req, writer);
    });

    service_->POST("/inst/echo", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_http_req(EV_INSTANCE_ECHO, req, writer);
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
