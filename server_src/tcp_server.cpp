#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <memory>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tcp_server.h"
#include <iostream>

namespace srv {
TcpServer::TcpServer(RequestObserver* primary_obsvr, const std::string &address, uint16_t port)
    : address_(address)
    , port_(port)
    , server_fd_(0)
    , is_running_(false)
    , primary_observer_(primary_obsvr) {
    CreateSocket();
}

TcpServer::~TcpServer() {
    Stop();
    close(server_fd_);
}

void TcpServer::Start() {
    BindSocket();
    StartListening();

    is_running_ = true;
    listener_ = std::thread(&TcpServer::Listen, this);
    CreateEpoll();
    for (int i = 0; i < kMaxWorkers; i++) {
        workers_[i] = std::thread(&TcpServer::ProcessEvents, this, i);
    }
}

void TcpServer::Stop() {
    if (!is_running_) return;

    is_running_ = false;
    listener_.join();
    for (int i = 0; i < kMaxWorkers; i++) {
        workers_[i].join();
        close(worker_fd_[i]);
    }
    close(server_fd_);
}

void TcpServer::SetPrimaryObserver(RequestObserver *obsvr) {
    primary_observer_ = obsvr;
}

void TcpServer::AddObserver(RequestObserver *obsvr) {
    observers_.push_back(obsvr);
}

void TcpServer::RemoveObserver(RequestObserver *obsvr) {
    for (auto it = observers_.begin(); it != observers_.end(); it++) {
        if (*it == obsvr) {
            observers_.erase(it);
            break;
        }
    }
}

void TcpServer::CreateSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (server_fd_ == -1) {
        throw std::runtime_error("Failed to create socket. " + std::string(strerror(errno)));
    }
}

void TcpServer::BindSocket() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (inet_pton(AF_INET, address_.c_str(), &(address.sin_addr.s_addr)) != 1) {
        throw std::runtime_error("Failed to convert ip address. " + std::string(strerror(errno)));
    }

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind socket to address and port. " + std::string(strerror(errno)));
    }
}

void TcpServer::StartListening() {
    if (listen(server_fd_, kBacklogSize) < 0) {
        throw std::runtime_error("Failed to start listening. " + std::string(strerror(errno)));
    }
}

void TcpServer::CreateEpoll() {
    for (int i = 0; i < kMaxWorkers; i++) {
        if ((worker_fd_[i] = epoll_create1(0)) < 0) {
            throw std::runtime_error("Epoll cannot be created. " + std::string(strerror(errno)));
        }
    }
}

void TcpServer::Listen() {
    sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    int client_fd;
    int current_worker = 0;

    while (is_running_) {
        client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);

        if (client_fd < 0) {
            continue;
        }

        epoll_event event;
        event.events = EPOLLIN;
        event.data.ptr = new EpollEventData{client_fd, ""};

        if (epoll_ctl(worker_fd_[current_worker], EPOLL_CTL_ADD, client_fd, &event) != 0) {
            throw std::runtime_error("Cannot create epoll. " + std::string(strerror(errno)));
        }

        current_worker++;
        if (current_worker == kMaxWorkers) current_worker = 0;
    }
}

void TcpServer::ProcessEvents(int current_worker) {
    int worker_fd = worker_fd_[current_worker];

    while (is_running_) {
        int events_ready = epoll_wait(worker_fd, worker_events[current_worker], kMaxEvents, 100);

        if (events_ready <= 0) {
            continue;
        }

        for (int i = 0; i < events_ready; i++) {
            epoll_event curr_event = worker_events[current_worker][i];
            EpollEventData* data = reinterpret_cast<EpollEventData*>(curr_event.data.ptr);

            if (curr_event.events & EPOLLIN) {
                HandleEpollInEvent(worker_fd, data);
            } else if (curr_event.events & EPOLLOUT) {
                HandleEpollOutEvent(worker_fd, data);
            } else {
                epoll_ctl(worker_fd_[current_worker], EPOLL_CTL_DEL, data->client_fd, nullptr);
                close(data->client_fd);
                delete data;
            }
        }
    }
}

void TcpServer::HandleEpollInEvent(int epoll_fd, EpollEventData* event_data) {
    char buffer[kBufferSize] = {0};
    ssize_t bytes_recv = recv(event_data->client_fd, buffer, kBufferSize, 0);

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
    }
    
    if (bytes_recv == 0) {
        std::cout << "---------------DISCONNECTED-------------\n";
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_data->client_fd, nullptr);
        close(event_data->client_fd);
        delete event_data;

        return;
    }

    // std::cout << "---------------GOT-------------\n" << buffer << "\n";

    std::string client_ip = GetClientIp(event_data->client_fd);


    event_data->buffer = primary_observer_->RequestHappen(buffer, client_ip);
    // event_data->buffer = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello bpb!\n\n";

    for (auto obsvr : observers_) {
        obsvr->RequestHappen(buffer, client_ip);
    }

    epoll_event event;
    event.events = EPOLLOUT;
    event.data.ptr = event_data;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event_data->client_fd, &event) != 0) {
        throw std::runtime_error("Something wrong with Epoll Ctl in EpollIn. " + std::string(strerror(errno)));
    }
}

void TcpServer::HandleEpollOutEvent(int epoll_fd, EpollEventData* event_data) {
    ssize_t bytes_sent = send(event_data->client_fd, event_data->buffer.c_str(), event_data->buffer.length(), 0);
    // std::cout << "---------------SENT-------------\n";
    // std::cout << event_data->buffer;

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        epoll_event event;
        event.events = EPOLLOUT;
        event.data.ptr = event_data;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_data->client_fd, &event) != 0) {
            throw std::runtime_error("Cannot create epoll. " + std::string(strerror(errno)));
        }
        
        return;
    }

    epoll_event event;
    event.events = EPOLLIN;

    event_data->buffer.clear();
    event.data.ptr = event_data;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event_data->client_fd, &event) != 0) {
        throw std::runtime_error("Something wrong with Epoll Ctl in EpollOut. " + std::string(strerror(errno)));
    }
}

std::string TcpServer::GetClientIp(int client_fd) {
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    getpeername(client_fd, (sockaddr*)&client_addr, &client_addr_size);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    return client_ip;
}
}