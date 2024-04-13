#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <memory>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

#include "server.h"

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
}

void SocketServer::Close() {
    if (!is_running_) return;

    is_running_ = false;
    listener_.join();
}

void SocketServer::CreateSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (server_fd_ == -1) {
        throw std::runtime_error(std::string("Failed to create socket. ") + strerror(errno));
    }
}

void SocketServer::BindSocket() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (inet_pton(AF_INET, address_.c_str(), &(address.sin_addr.s_addr)) != 1) {
        throw std::runtime_error(std::string("Failed to convert ip address. ") + strerror(errno));
    }

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error(std::string("Failed to bind socket to address and port. ") + strerror(errno));
    }
}

void SocketServer::StartListening() {
    if (listen(server_fd_, kBacklogSize) < 0) {
        throw std::runtime_error(std::string("Failed to start listening. ") + strerror(errno));
    }
}

void SocketServer::Listen() {
    sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    int client_fd;

    while (is_running_) {
        client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);

        if (client_fd < 0) {
            continue;
        }

        char buffer[kBufferSize] = {0};
        int valread = read(client_fd, buffer, kBufferSize);
        // std::cout << "----------------GOT----------------\n" << buffer << "\n";
        if (connection_logger_) {
            connection_logger_->LogConnected(buffer);
        }

        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello " + std::string(buffer) + "!\n\n";
        send(client_fd, response.c_str(), response.length(), 0);
        // std::cout << "----------------Response sent----------------\n";

        close(client_fd);
    }
}
}