#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "http_message.h"

namespace srv {
    class HttpResponse : public HttpMessage {
       public:
        static HttpResponse Parse(const std::string& response_string);

        virtual std::string Build() override;

        uint16_t GetStatusCode() const;
        std::string GetStatusCodeMean() const;

       protected:
        uint16_t status_code_;
        std::string status_code_mean_;
    };
} // namespace srv


#endif  // _HTTP_RESPONSE_H_
