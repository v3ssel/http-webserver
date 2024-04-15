#include <algorithm>
#include "http_request.h"

namespace srv {
HttpRequest HttpRequest::Parse(const std::string &request_string) {
    HttpRequest parsed;

    size_t method_ends = request_string.find(' ');
    size_t resource_ends = request_string.find(' ', method_ends + 1);

    parsed.http_method_ = request_string.substr(0, method_ends);
    parsed.resource_ = request_string.substr(method_ends + 1, resource_ends - method_ends - 1);

    std::string version_str = request_string.substr(resource_ends + 6, 3);
    if (version_str[0] == '3') {
        parsed.version_ = HttpVersion::HTTP_3_0;
    } else if (version_str[0] == '2') {
        parsed.version_ = HttpVersion::HTTP_2_0;
    } else if (version_str[0] == '1' && version_str[2] == '1') {
        parsed.version_ = HttpVersion::HTTP_1_1;
    } else if (version_str[0] == '1' && version_str[2] == '0') {
        parsed.version_ = HttpVersion::HTTP_1_0;
    } else {
        parsed.version_ = HttpVersion::HTTP_0_9;
    }

    size_t first_line_ends = request_string.find('\n');
    size_t header_ends = request_string.find("\n\n");

    for (size_t i = first_line_ends + 1; i < header_ends;) {
        size_t line_ends = request_string.find('\n', i);
        size_t separator = request_string.find(':', i);

        std::string header = request_string.substr(i, separator - i);
        std::string value = request_string.substr(separator + 1, line_ends - separator - 1);
        
        value.erase(0, value.find_first_not_of(' '));
        value.erase(value.find_last_not_of(' ') + 1);

        parsed.AddHeader(header, value);
        i = line_ends + 1;
    }

    parsed.body_ = request_string.substr(header_ends + 2);

    return parsed;
}

HttpRequest::HttpRequest(const std::string &body) : HttpMessage() {
    if (!body.empty()) {
        *this = Parse(body);
    }
}

std::string HttpRequest::Build() {
    std::string built = http_method_ + " " + resource_ + " HTTP/";
    if (version_ == HttpVersion::HTTP_3_0) {
        built += "3.0\n";
    } else if (version_ == HttpVersion::HTTP_2_0) {
        built += "2.0\n";
    } else if (version_ == HttpVersion::HTTP_1_1) {
        built += "1.1\n";
    } else if (version_ == HttpVersion::HTTP_1_0) {
        built += "1.0\n";
    } else {
        built += "0.9\n";
    }
    for (auto& [k, v] : headers_) {
        built += k + ": " + v + "\n";
    }
    built += '\n';
    built += body_;

    return built;
}

std::string HttpRequest::GetHttpMethod() const {
    return http_method_;
}

std::string HttpRequest::GetRequestedResource() const {
    return resource_;
}
}