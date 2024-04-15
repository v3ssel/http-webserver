#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "http_message.h"

namespace srv {
    class HttpRequest : public HttpMessage {
       public:
        static HttpRequest Parse(const std::string& request_string);

        HttpRequest(const std::string& body = "");
        virtual std::string Build() override;

        std::string GetHttpMethod() const;
        std::string GetRequestedResource() const;

       protected:
        std::string http_method_;
        std::string resource_; 
    };
} // namespace srv


#endif  // _HTTP_REQUEST_H_
