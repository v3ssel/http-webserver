#include "http_server.h"

#include "http_request.h"
#include "http_response.h"

namespace srv {
HttpServer::HttpServer(const std::string &address, uint16_t port) {
    server_ = std::make_unique<TcpServer>(this, address, port);
    binder_ = std::make_unique<ResourceBinder>();
    connection_logger_ = nullptr;
}

std::string HttpServer::RequestHappen(const std::string &request_str, const std::string& client) {
    HttpRequest request = HttpRequest::Parse(request_str);

    if (connection_logger_) {
        connection_logger_->LogConnected(client);
    }
    
    srv::HttpResponse response = binder_->Execute(request);
    return response.Build();
}

void HttpServer::Start() {
    server_->Start();
}

void HttpServer::Stop() {
    server_->Stop();
}

void HttpServer::Bind(const std::string &resource_path,
                      const std::string &method,
                      ResourceBinder::function_handler handler) {
    binder_->Bind(resource_path, method, handler);
}

void HttpServer::Unbind(const std::string &resource_path, const std::string &method) {
    binder_->Unbind(resource_path, method);
}

void HttpServer::SetConnectionLogger(BaseConnectionLogger *clogger)
{
    connection_logger_ = std::unique_ptr<BaseConnectionLogger>(clogger);
}
}