#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include <thread>

#include <sys/epoll.h>

#include "base_connection_logger.h"

namespace srv {
    class SocketServer {
       public:
        static constexpr int kBacklogSize = 10;
        static constexpr int kBufferSize = 2048;
        static constexpr int kMaxWorkers = 10;
        static constexpr int kMaxEvents = 10000;

        SocketServer(const std::string& address = "127.0.0.1", uint16_t port = 8088, BaseConnectionLogger* connection_logger = nullptr);
        ~SocketServer();

        void Start();
        void Close();

       private:
        void CreateSocket();
        void BindSocket();
        void StartListening();
        void CreateEpoll();

        void Listen();
        void ProcessEvents(int current_worker);

        void HandleEpollInEvent(int epoll_fd, int client_fd);
        void HandleEpollOutEvent(int epoll_fd, int client_fd);

        std::string address_;
        uint16_t port_;
        std::unique_ptr<BaseConnectionLogger> connection_logger_;

        int server_fd_;
        bool is_running_;

        std::thread listener_;

        int worker_fd_[kMaxWorkers];
        std::thread workers_[kMaxWorkers];
        epoll_event worker_events[kMaxWorkers][kMaxEvents];
    };
}

#endif  // _SERVER_H_
