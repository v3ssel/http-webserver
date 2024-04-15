#include "http_message.h"

namespace srv {
HttpMessage::HttpMessage(HttpVersion version) {
    version_ = version;
}

typename HttpMessage::self& HttpMessage::AddHeader(const std::string &key, const std::string &value) {
    headers_.insert({key, value});
    
    return *this;
}

typename HttpMessage::self& HttpMessage::RemoveHeader(const std::string &key) {
    headers_.erase(key);
    
    return *this;
}

std::unordered_map<std::string, std::string> HttpMessage::GetHeaders() const {
    return headers_;
}

std::string HttpMessage::GetHeaderValue(const std::string &key) const {
    return headers_.at(key);
}

typename HttpMessage::self& HttpMessage::UpdateHeader(const std::string &key, const std::string &new_value) {
    headers_[key] = new_value;
    
    return *this;
}

typename HttpMessage::self& HttpMessage::SetHttpVersion(HttpVersion version) {
    version_ = version;
    
    return *this;
}

HttpVersion HttpMessage::GetHttpVersion() const {
    return version_;
}

typename HttpMessage::self& HttpMessage::SetBody(const std::string &body, const std::string& content_type) {
    body_ = body;
    AddHeader("Content-Type", content_type);
    AddHeader("Content-Length", std::to_string(body.length()));

    return *this;
}

std::string HttpMessage::GetBody() const {
    return body_;
}

typename HttpMessage::self& HttpMessage::EraseBody() {
    body_ = std::string();
    RemoveHeader("Content-Type");
    RemoveHeader("Content-Length");
    
    return *this;
}
}