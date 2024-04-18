#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <string>
#include <thread>

#include <sys/epoll.h>

#include "base_connection_logger.h"
#include "request_observer.h"

namespace srv {
    class TcpServer {
        struct EpollEventData {
            int client_fd;
            std::string buffer;
        };
       public:
        static constexpr int kBacklogSize = 10;
        static constexpr int kBufferSize = 8192;
        static constexpr int kMaxWorkers = 10;
        static constexpr int kMaxEvents = 10000;

        TcpServer(const std::string& address = "127.0.0.1", uint16_t port = 8088, RequestObserver* obsvr = nullptr);
        ~TcpServer();

        void Start();
        void Stop();

        void SetObserver(RequestObserver* obsvr);

       private:
        void CreateSocket();
        void BindSocket();
        void StartListening();
        void CreateEpoll();

        void Listen();
        void ProcessEvents(int current_worker);

        void HandleEpollInEvent(int epoll_fd, EpollEventData* event_data);
        void HandleEpollOutEvent(int epoll_fd, EpollEventData* event_data);

        std::string address_;
        uint16_t port_;
        std::unique_ptr<BaseConnectionLogger> connection_logger_;

        int server_fd_;
        bool is_running_;

        std::thread listener_;

        int worker_fd_[kMaxWorkers];
        std::thread workers_[kMaxWorkers];
        epoll_event worker_events[kMaxWorkers][kMaxEvents];

        RequestObserver* observer_;
    };
}

#endif  // _TCP_SERVER_H_
