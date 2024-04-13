#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include <thread>

namespace srv {
    class SocketServer {
       public:
        static constexpr int kBacklogSize = 10;
        static constexpr int kBufferSize = 2048;

        SocketServer(const std::string& address = "127.0.0.1", uint16_t port = 8088);
        ~SocketServer();

        void Start();
        void Close();

       private:
        void CreateSocket();
        void BindSocket();
        void StartListening();

        void Listen();

        std::string address_;
        uint16_t port_;

        int server_fd_;
        bool is_running_;

        std::thread listener_;
    };
}

#endif  // _SERVER_H_
