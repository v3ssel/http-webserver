#include <algorithm>

#include "http_request.h"

namespace srv {
HttpRequest HttpRequest::Parse(const std::string &request_string) {
    HttpRequest parsed;

    size_t method_ends = request_string.find(' ');
    size_t resource_ends = request_string.find(' ', method_ends + 1);

    parsed.http_method_ = request_string.substr(0, method_ends);
    parsed.resource_ = request_string.substr(method_ends + 1, resource_ends - method_ends - 1);
    parsed.version_ = StringToHttpVersion(request_string.substr(resource_ends + 1, 3));

    size_t first_line_ends = request_string.find('\n');
    size_t header_ends = request_string.find("\n\n");

    parsed.ParseHeadersString(request_string.substr(first_line_ends + 1, header_ends - first_line_ends));

    parsed.SetBody(request_string.substr(header_ends + 2),
                   parsed.ContainsHeader("Content-Type") ? parsed.GetHeaderValue("Content-Type") : "text/plain");

    return parsed;
}

HttpRequest::HttpRequest(const std::string &body) : HttpMessage() {
    if (!body.empty()) {
        *this = Parse(body);
    }
}

std::string HttpRequest::Build() {
    std::string built = http_method_ + " " + resource_;
    built += " " + HttpVersionToString(version_) + "\n";
    
    built += HeadersToString() + '\n';
    built += body_;

    return built;
}

std::string HttpRequest::GetHttpMethod() const {
    return http_method_;
}

HttpRequest::self_pointer HttpRequest::SetHttpMethod(const std::string& method) {
    http_method_ = method;
    
    return this;
}

std::string HttpRequest::GetRequestedResource() const {
    return resource_;
}

HttpRequest::self_pointer HttpRequest::SetResource(const std::string &resource) {
    resource_ = resource;
    
    return this;
}
}