#include "service/license_server.h"
#include "domain/handler/auth_req_handler.h"
#include "domain/handler/instance_rel_handler.h"
#include "domain/event_id.h"

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

bool handle_http_msg(EventId id, const std::string& msg)
{
    auto iter = all_handlers.find(id);
    if (iter != all_handlers.end())
    {
        return iter->second->handle(msg);
    }

    return false;
}

void send_http_rsp(const HttpResponseWriterPtr& writer, const std::string& status, const std::string& message)
{
    json response;
    response["status"] = status;
    response["message"] = message;
    writer->End(response.dump());
}

}

LicenseServer::LicenseServer(int port) : port_{port}, service_{new ::hv::HttpService()} 
{
    service_->POST("/auth/license",  [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        json response;
        try 
        {            
            if (!handle_http_msg(EV_AUTHRIZATION_REQ, req->body))
            {
                send_http_rsp(writer, "faliure", "Device not authorized or max instance limit reached.");
                return;
            }
            send_http_rsp(writer, "success", "Authorization successful.");
        } 
        catch (const std::exception& e) 
        {
            send_http_rsp(writer, "faliure", "Invalid request format.");
        }
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
