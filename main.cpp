#include <iostream>

#include "server_src/http_server.h"
#include "loggers/connection_logger.h"
#include "controllers/home_controller.h"


int main(int argc, char** argv) try {
    std::string kAddress = "127.0.0.1";

    if (argc < 2) {
        std::cout << "usage: ./server [port]\n";
        return -1;
    }

    int port = std::stoi(argv[1]);
    if (port < 1025 || port > 65535) {
        throw std::invalid_argument("");
    }


    srv::HttpServer srv(kAddress, port);
    
    srv::HomeController controller;
    srv.Bind("/", "GET", std::bind(&srv::HomeController::GetHome, &controller, std::placeholders::_1));
    // pass header "Name: <name>" as parameter
    srv.Bind("/hello", "GET", std::bind(&srv::HomeController::GetHello, &controller, std::placeholders::_1));

    std::cout << "Starting http://" << kAddress << ":" << port << " server.\n";
    srv.Start();
    std::cout << "Server started.\n";

    std::cout << "Type \'q\' to exit\n";
    std::string cmd;
    while (std::cin >> cmd, cmd != "q") {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Shutting down the server.\n";
    srv.Stop();
    std::cout << "Server successfully exit.\n";

} catch (std::invalid_argument&) {
    std::cout << "Invalid port number.\n";

} catch (std::exception& ex) {
    std::cout << "Critical error occured.\n" << ex.what() << "\n";
}
