#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "tcp_server.h"
#include "base_connection_logger.h"
#include "resource_binder.h"
#include "request_observer.h"

namespace srv {
    class HttpServer : public RequestObserver {
       public:
        HttpServer(const std::string& address, uint16_t port);
        
        std::string RequestHappen(const std::string& request) override;

        void Start();
        void Stop();

        void Bind(const std::string& resource_path, const std::string& method, ResourceBinder::function_handler handler);
        void Unbind(const std::string& resource_path, const std::string& method);

        void SetConnectionLogger(BaseConnectionLogger* clogger);

       private:
        std::unique_ptr<TcpServer> server_;
        std::unique_ptr<ResourceBinder> binder_;
        std::unique_ptr<BaseConnectionLogger> connection_logger_;
    };
}

#endif  //  _HTTP_SERVER_H_
