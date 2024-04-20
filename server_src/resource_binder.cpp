#include "resource_binder.h"

namespace srv {
ResourceBinder::ResourceBinder() {
    using namespace std::string_literals;
    
    err_handler_ = [](const HttpRequest& req, int error_code) -> HttpResponse {
        HttpResponse response;
        response.SetStatusCodeMean(error_code == 404 ? "Not Found" : "Method Not Allowed")
                ->SetStatusCode(error_code)
                ->SetHttpVersion(req.GetHttpVersion())
                ->SetBody(
            "Default error handler invoked on ["s +
            req.GetHttpMethod() + "] \'"s + req.GetRequestedResource() +
            "\' with HTTP error code "s + std::to_string(error_code) + "."s,
            "text/plain");

        return response;
    };
}

void ResourceBinder::Bind(const std::string &resource_path, const std::string &method, function_handler handler) {
    methods_to_handlers_[resource_path.substr(0, resource_path.find('?'))][method] = handler;
}

void ResourceBinder::Unbind(const std::string &resource_path, const std::string &method) {
    if (methods_to_handlers_.count(resource_path) > 0 &&    
        methods_to_handlers_[resource_path].count(method) > 0) {
        methods_to_handlers_[resource_path].erase(method);

        if (methods_to_handlers_[resource_path].empty()) {
            methods_to_handlers_.erase(resource_path);
        }
    }
}

HttpResponse ResourceBinder::Execute(const HttpRequest &request) {
    if (methods_to_handlers_.count(request.GetRequestedResource()) == 0) {
        return err_handler_(request, 404);
    }

    if (methods_to_handlers_[request.GetRequestedResource()].count(request.GetHttpMethod()) == 0) {
        return err_handler_(request, 405);
    }

    return methods_to_handlers_[request.GetRequestedResource()][request.GetHttpMethod()](request);
}

void ResourceBinder::SetErrorCodeHandler(error_handler handler) {
    err_handler_ = std::move(handler);
}
}
