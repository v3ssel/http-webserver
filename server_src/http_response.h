#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "http_message.h"

namespace srv {
    class HttpResponse : public HttpMessage {
       public:
        virtual std::string Build() override;

       protected:
        uint16_t status_code_;
        std::string status_code_mean_;
    };
} // namespace srv


#endif  // _HTTP_RESPONSE_H_
