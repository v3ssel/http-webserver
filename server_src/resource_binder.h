#ifndef _RESOURCE_BINDER_H_
#define _RESOURCE_BINDER_H_

#include <unordered_map>
#include <functional>
#include <string>

#include "../http/http_request.h"
#include "../http/http_response.h"

namespace srv {
    class ResourceBinder {
       public:
        using function_handler = std::function<HttpResponse(const HttpRequest&)>;
        using error_handler = std::function<HttpResponse(const HttpRequest&, int)>;
        
        ResourceBinder();

        virtual void Bind(const std::string& resource_path, const std::string& method, function_handler handler);
        virtual void Unbind(const std::string& resource_path, const std::string& method);
        virtual HttpResponse Execute(const HttpRequest &request);

        void SetErrorCodeHandler(error_handler handler);
        
       protected:
        using method_to_function = std::unordered_map<std::string, function_handler>;
        using resource_to_method = std::unordered_map<std::string, method_to_function>;

        resource_to_method methods_to_handlers_;
        error_handler err_handler_;

    }; 
} // namespace srv

#endif  //  _RESOURCE_BINDER_H_
