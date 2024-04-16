#include "http_message.h"

namespace srv {
HttpMessage::HttpMessage(HttpVersion version) {
    version_ = version;
}

typename HttpMessage::self_pointer HttpMessage::AddHeader(const std::string &key, const std::string &value) {
    headers_.insert({key, value});
    
    return this;
}

typename HttpMessage::self_pointer HttpMessage::RemoveHeader(const std::string &key) {
    headers_.erase(key);
    
    return this;
}

std::unordered_map<std::string, std::string> HttpMessage::GetHeaders() const {
    return headers_;
}

std::string HttpMessage::GetHeaderValue(const std::string &key) const {
    return headers_.at(key);
}

typename HttpMessage::self_pointer HttpMessage::UpdateHeader(const std::string &key, const std::string &new_value) {
    headers_[key] = new_value;
    
    return this;
}

typename HttpMessage::self_pointer HttpMessage::SetHttpVersion(HttpVersion version) {
    version_ = version;
    
    return this;
}

HttpVersion HttpMessage::GetHttpVersion() const {
    return version_;
}

typename HttpMessage::self_pointer HttpMessage::SetBody(const std::string &body, const std::string& content_type) {
    body_ = body;
    AddHeader("Content-Type", content_type);
    AddHeader("Content-Length", std::to_string(body.length()));

    return this;
}

std::string HttpMessage::GetBody() const {
    return body_;
}

typename HttpMessage::self_pointer HttpMessage::EraseBody() {
    body_ = std::string();
    RemoveHeader("Content-Type");
    RemoveHeader("Content-Length");
    
    return this;
}

std::string HttpMessage::HttpVersionToString(HttpVersion version) {
    std::string version_str = "HTTP/";

    if (version == HttpVersion::HTTP_3_0) {
        version_str += "3.0";
    } else if (version == HttpVersion::HTTP_2_0) {
        version_str += "2.0";
    } else if (version == HttpVersion::HTTP_1_1) {
        version_str += "1.1";
    } else if (version == HttpVersion::HTTP_1_0) {
        version_str += "1.0";
    } else {
        version_str += "0.9";
    }

    return version_str;
}

HttpVersion HttpMessage::StringToHttpVersion(const std::string& version_str) {
    if (version_str.length() <= 7) return HttpVersion::HTTP_1_1;

    if (version_str[5] == '3' && version_str[7] == '0') {
        return HttpVersion::HTTP_3_0;
    } else if (version_str[5] == '2' && version_str[7] == '0') {
        return HttpVersion::HTTP_2_0;
    } else if (version_str[5] == '1' && version_str[7] == '1') {
        return HttpVersion::HTTP_1_1;
    } else if (version_str[5] == '1' && version_str[7] == '0') {
        return HttpVersion::HTTP_1_0;
    } else if (version_str[5] == '0' && version_str[7] == '9') {
        return HttpVersion::HTTP_0_9;
    }

    return HttpVersion::HTTP_1_1;
}

std::string HttpMessage::HeadersToString() const {
    std::string output;
    
    for (auto& header : headers_) {
        output += header.first + ": " + header.second + "\n";
    }

    return output;
}

void HttpMessage::ParseHeadersString(const std::string &headers) {
    for (size_t i = 0; i < headers.length();) {
        size_t line_ends = headers.find('\n', i);
        size_t separator = headers.find(':', i);

        std::string header = headers.substr(i, separator - i);
        std::string value = headers.substr(separator + 1, line_ends - separator - 1);
        
        // trim value string
        value.erase(0, value.find_first_not_of(' '));
        value.erase(value.find_last_not_of(' ') + 1);

        AddHeader(header, value);
        i = line_ends + 1;
    }
}
}