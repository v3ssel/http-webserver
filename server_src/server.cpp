#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <memory>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include <iostream>

namespace srv {
SocketServer::SocketServer(const std::string &address, uint16_t port, BaseConnectionLogger* connection_logger)
    : address_(address)
    , port_(port)
    , server_fd_(0)
    , is_running_(false) {
    connection_logger_ = std::unique_ptr<BaseConnectionLogger>(connection_logger);
    CreateSocket();
}

SocketServer::~SocketServer() {
    Close();
    close(server_fd_);
}

void SocketServer::Start() {
    BindSocket();
    StartListening();

    is_running_ = true;
    listener_ = std::thread(&SocketServer::Listen, this);
    CreateEpoll();
    for (int i = 0; i < kMaxWorkers; i++) {
        workers_[i] = std::thread(&SocketServer::ProcessEvents, this, i);
    }
}

void SocketServer::Close() {
    if (!is_running_) return;

    is_running_ = false;
    listener_.join();
    for (int i = 0; i < kMaxWorkers; i++) {
        workers_[i].join();
        close(worker_fd_[i]);
    }
    close(server_fd_);
}

void SocketServer::CreateSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (server_fd_ == -1) {
        throw std::runtime_error("Failed to create socket. " + std::string(strerror(errno)));
    }
}

void SocketServer::BindSocket() {
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

void SocketServer::StartListening() {
    if (listen(server_fd_, kBacklogSize) < 0) {
        throw std::runtime_error("Failed to start listening. " + std::string(strerror(errno)));
    }
}

void SocketServer::CreateEpoll() {
    for (int i = 0; i < kMaxWorkers; i++) {
        if ((worker_fd_[i] = epoll_create1(0)) < 0) {
            throw std::runtime_error("Epoll cannot be created. " + std::string(strerror(errno)));
        }
    }
}

void SocketServer::Listen() {
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
        event.data.fd = client_fd;

        if (epoll_ctl(worker_fd_[current_worker], EPOLL_CTL_ADD, client_fd, &event) != 0) {
            throw std::runtime_error("Cannot create epoll. " + std::string(strerror(errno)));
        }

        current_worker++;
        if (current_worker == kMaxWorkers) current_worker = 0;
    }
}

void SocketServer::ProcessEvents(int current_worker) {
    int worker_fd = worker_fd_[current_worker];

    while (is_running_) {
        int events_ready = epoll_wait(worker_fd, worker_events[current_worker], kMaxEvents, 100);

        if (events_ready <= 0) {
            continue;
        }

        for (int i = 0; i < events_ready; i++) {
            epoll_event curr_event = worker_events[current_worker][i];
            int client_fd = curr_event.data.fd;

            if (curr_event.events & EPOLLIN) {
                HandleEpollInEvent(worker_fd, client_fd);
            } else if (curr_event.events & EPOLLOUT) {
                HandleEpollOutEvent(worker_fd, client_fd);
            } else {
                epoll_ctl(worker_fd_[current_worker], EPOLL_CTL_DEL, client_fd, nullptr);
                close(client_fd);
            }
        }
    }
}

void SocketServer::HandleEpollInEvent(int epoll_fd, int client_fd) {
    char buffer[kBufferSize] = {0};
    ssize_t bytes_recv = recv(client_fd, buffer, kBufferSize, 0);
    
    if (bytes_recv == 0) {
        std::cout << "---------------DISCONNECTED-------------\n";
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
        close(client_fd);
        return;
    }

    std::cout << "---------------GOT-------------\n" << buffer << "\n";

    if (connection_logger_) {
        connection_logger_->LogConnected(buffer);
    }

    epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &event) != 0) {
        throw std::runtime_error("Something wrong with Epoll Ctl in EpollIn. " + std::string(strerror(errno)));
    }
}

void SocketServer::HandleEpollOutEvent(int epoll_fd, int client_fd) {
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello bpb!\n\n";
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    std::cout << "---------------SENT-------------\n";
    std::cout << response;

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &event) != 0) {
        throw std::runtime_error("Something wrong with Epoll Ctl in EpollOut. " + std::string(strerror(errno)));
    }
}
}