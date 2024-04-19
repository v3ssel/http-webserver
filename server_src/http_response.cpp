#include "http_response.h"

namespace srv {
HttpResponse HttpResponse::Parse(const std::string &response_string) {
    HttpResponse parsed;

    size_t version_ends = response_string.find(' ');
    size_t status_code_ends = response_string.find(' ', version_ends + 1);
    size_t first_line_ends = response_string.find('\n');

    parsed.version_ = StringToHttpVersion(response_string.substr(0, version_ends));
    parsed.status_code_ = std::stoi(response_string.substr(version_ends + 1, status_code_ends - version_ends - 1));
    parsed.status_code_mean_ = response_string.substr(status_code_ends + 1, first_line_ends - status_code_ends - 1);

    size_t header_ends = response_string.find("\n\n");
    parsed.ParseHeadersString(response_string.substr(first_line_ends + 1, header_ends - first_line_ends));

    parsed.SetBody(response_string.substr(header_ends + 2), 
                   parsed.ContainsHeader("Content-Type") ? parsed.GetHeaderValue("Content-Type") : "text/plain");

    return parsed;
}

std::string HttpResponse::Build() {
    std::string output = HttpVersionToString(version_) + " " + std::to_string(status_code_) + " " + status_code_mean_ + "\n";

    output += HeadersToString() + "\n";
    output += body_;
    
    return output;
}

uint16_t HttpResponse::GetStatusCode() const {
    return status_code_;
}

typename HttpResponse::self_pointer HttpResponse::SetStatusCode(uint16_t status_code) {
    status_code_ = status_code;

    return this;
}

std::string HttpResponse::GetStatusCodeMean() const {
    return status_code_mean_;
}

typename HttpResponse::self_pointer HttpResponse::SetStatusCodeMean(const std::string &status_code_mean) {
    status_code_mean_ = status_code_mean;

    return this;
}
}