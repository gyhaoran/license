#include "service/license_server.h"
#include "domain/handler/auth_req_handler.h"
#include "domain/handler/instance_rel_handler.h"
#include "domain/event/event.h"

#include <hv/HttpServer.h>
#include <hv/HttpResponseWriter.h>
#include <hv/hasync.h>

#include <functional>
#include <map>

using json = nlohmann::json;

namespace lic
{

namespace
{

std::map<EventId, BaseMsgHandler*> all_handlers = {
    {EV_AUTHRIZATION_REQ,     new AuthReqHandler()},
    {EV_INSTANCE_REL,         new InstanceRelHandler()},
};

std::string handle_http_msg(EventId id, const std::string& msg)
{
    auto iter = all_handlers.find(id);
    if (iter != all_handlers.end())
    {
        Event event(id, msg);
        iter->second->handle(event);

        return event.get_rsp_msg();
    }

    return R"({"status": "Failure", "message": "unkonwn"})";
}

void send_http_rsp(const HttpResponseWriterPtr& writer, const std::string& rsp)
{
    writer->End(rsp);
}

void handle_authen_req(const std::string& msg, const HttpResponseWriterPtr& writer)
{
    try 
    {
        auto rsp = handle_http_msg(EV_AUTHRIZATION_REQ, msg);
        writer->End(rsp);
    } 
    catch (const std::exception& e) 
    {
        writer->End(R"({"status": "Failure", "message": "unkonwn"})");
    }
}

void handle_inst_release(const std::string& msg, const HttpResponseWriterPtr& writer)
{
    try 
    {
        auto rsp = handle_http_msg(EV_INSTANCE_REL, msg);
        writer->End(rsp);
    } 
    catch (const std::exception& e) 
    {
        writer->End(R"({"status": "Failure", "message": "unkonwn"})");
    }
}

} // namespace

LicenseServer::LicenseServer(int port) : port_{port}, service_{new ::hv::HttpService()} 
{
    service_->POST("/auth/license",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_authen_req(req->body, writer);
    });

    service_->POST("/inst/rel",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        handle_inst_release(req->body, writer);
    });
}

LicenseServer::~LicenseServer()
{
    delete service_;
    service_ = nullptr;
}

void LicenseServer::run()
{
    hv::HttpServer server;
    server.service = service_;
    server.port = port_;

    server.run();

    ::hv::async::cleanup();
}

} // namespace lic
