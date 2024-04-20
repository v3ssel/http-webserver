#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "connection_logger.h"

namespace srv {
ConnectionLogger::ConnectionLogger(const std::string &file_name)
    : file_name_(file_name) {}

void ConnectionLogger::LogConnected(const std::string &client_name) {
    std::ofstream file(file_name_, std::ios::app);
    
    auto now = std::chrono::high_resolution_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    auto now_time = std::chrono::high_resolution_clock::to_time_t(now);
    std::tm* tm = std::localtime(&now_time);

    std::stringstream log_line;
    log_line << "[" 
                    << std::put_time(tm, "%F %T.")
                    << std::setw(3)
                    << std::setfill('0')
                    << ms 
             << "] "
             << client_name << "\n";
    
    file << log_line.str();
}
}