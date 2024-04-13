#ifndef _CONNECTION_LOGGER_H_
#define _CONNECTION_LOGGER_H_

#include <string>
#include "base_connection_logger.h"

namespace srv {
class ConnectionLogger : public BaseConnectionLogger {
   public:
    ConnectionLogger(const std::string& file_name = "log.txt");

    virtual void LogConnected(const std::string& client_name);

   protected:
    std::string file_name_;
};
}

#endif  //  _CONNECTION_LOGGER_H_
