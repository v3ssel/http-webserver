#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "http_message.h"

namespace srv {
    class HttpResponse : public HttpMessage {
       public:
        using self = HttpResponse;
        using self_pointer = self*;

        static HttpResponse Parse(const std::string& response_string);

        virtual std::string Build() override;

        uint16_t GetStatusCode() const;
        self_pointer SetStatusCode(uint16_t status_code);

        std::string GetStatusCodeMean() const;
        self_pointer SetStatusCodeMean(const std::string& status_code_mean);

       protected:
        uint16_t status_code_;
        std::string status_code_mean_;
    };
} // namespace srv


#endif  // _HTTP_RESPONSE_H_
