#include <iostream>

#include <thread>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "usage: client [name] [port] [delay(in seconds)]\n";
        return -1;
    }

    std::string kAddress = "127.0.0.1";
    
    try {
        std::string client_name = argv[1];
        
        int port = std::stoi(argv[2]);
        if (port < 0 || port > 65535) throw std::invalid_argument("");
        int delay = std::stoi(argv[3]);
        if (delay < 0 || delay > 7200) throw std::invalid_argument("");

        // connect sleep connect sleep
            int client_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (client_fd == -1) {
                throw std::runtime_error("Cannot create client socket. " + std::string(strerror(errno)) + "\n");
            }

            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);
            
            if (inet_pton(AF_INET, kAddress.c_str(), &address.sin_addr.s_addr) <= 0) {
                throw std::runtime_error("Invalid ip address. " + std::string(strerror(errno)) + "\n");
            }

            if (connect(client_fd, (sockaddr*)&address, sizeof(address)) == -1) {
                throw std::runtime_error("Cannot establish connection with http://" + kAddress + ":" + std::to_string(port) + ". " + std::string(strerror(errno)) + "\n");
            }
                
        while (true) {
            int sent = send(client_fd, client_name.c_str(), client_name.length(), 0);
            if (sent != client_name.length()) {
                std::cout << "Error in data sending. Trying again.\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }

        close(client_fd);

    } catch (std::invalid_argument) {
        std::cout << "Invalid port or delay number.";
    } catch (const std::exception& e) {
        std::cout << "Critical error occured.\n" << e.what() << "\n";
    }

    return 0;
}
