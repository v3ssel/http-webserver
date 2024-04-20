#ifndef _HOME_CONTROLLER_H_
#define _HOME_CONTROLLER_H_

#include "../http/http_request.h"
#include "../http/http_response.h"

namespace srv {
    class HomeController {
       public:
        HttpResponse GetHome(const HttpRequest& request);
        HttpResponse GetHello(const HttpRequest& request);
    };
}

#endif  //  _HOME_CONTROLLER_H_
