#ifndef _HTTP_MESSAGE_H_
#define _HTTP_MESSAGE_H_

#include <string>
#include <unordered_map>
#include "http_version.h"

namespace srv {
    class HttpMessage {
       public:
        using self = HttpMessage;
        using self_pointer = self*;

        HttpMessage(HttpVersion version = HttpVersion::HTTP_1_1);
        virtual ~HttpMessage() = default;

        virtual std::string Build() = 0;

        self_pointer AddHeader(const std::string& key, const std::string& value = "");
        self_pointer RemoveHeader(const std::string& key);

        std::unordered_map<std::string, std::string> GetHeaders() const;
        std::string GetHeaderValue(const std::string& key) const;
        self_pointer UpdateHeader(const std::string& key, const std::string& new_value);

        self_pointer SetHttpVersion(HttpVersion version);
        HttpVersion GetHttpVersion() const;

        self_pointer SetBody(const std::string& body, const std::string& content_type);
        std::string GetBody() const;
        self_pointer EraseBody();

        static std::string HttpVersionToString(HttpVersion version);
        static HttpVersion StringToHttpVersion(const std::string& version_str);

       protected:
        std::string HeadersToString() const;
        void ParseHeadersString(const std::string& headers);

        HttpVersion version_;
        std::unordered_map<std::string, std::string> headers_;

        std::string body_;
    };
} // namespace srv


#endif  // _HTTP_MESSAGE_H_
