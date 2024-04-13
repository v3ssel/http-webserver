#ifndef _BASE_CONNECTION_LOGGER_H_
#define _BASE_CONNECTION_LOGGER_H_

#include <string>

namespace srv {
class BaseConnectionLogger {
   public:
    virtual void LogConnected(const std::string& client_name) = 0;
};
}

#endif  //  _BASE_CONNECTION_LOGGER_H_
