#ifndef _REQUEST_OBSERVER_H_
#define _REQUEST_OBSERVER_H_

#include <string>

namespace srv {
    class RequestObserver {
       public:
        virtual std::string RequestHappen(const std::string& request, const std::string& client) = 0;
        virtual ~RequestObserver() = default;
    };
} // namespace srv


#endif  //  _REQUEST_OBSERVER_H_
