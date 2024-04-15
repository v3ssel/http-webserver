#include <gtest/gtest.h>

#include "../server_src/http_request.h"

TEST(HttpRequest, Parse) {
    std::string request = 
        "GET /some_resource/indentifier HTTP/1.1\n"
        "Host: example.com\n"
        "User-Agent: curl/7.81.0\n"
        "Accept: */*\n"
        "Content-Length: 4\n"
        "Content-Type: application/x-www-form-urlencoded\n\n"
        "{\"somedata\":\"and_its_value\"}";
    
    srv::HttpRequest parsed = srv::HttpRequest::Parse(request);
    auto headers = parsed.GetHeaders();   

    EXPECT_EQ(parsed.GetHttpVersion(), srv::HttpVersion::HTTP_1_1);
    EXPECT_EQ(parsed.GetHttpMethod(), "GET");
    EXPECT_EQ(parsed.GetRequestedResource(), "/some_resource/indentifier");

    EXPECT_EQ(headers["Host"], "example.com");
    EXPECT_EQ(headers["User-Agent"], "curl/7.81.0");
    EXPECT_EQ(headers["Accept"], "*/*");
    EXPECT_EQ(headers["Content-Length"], "4");
    EXPECT_EQ(headers["Content-Type"], "application/x-www-form-urlencoded");
    EXPECT_EQ(parsed.GetBody(), "{\"somedata\":\"and_its_value\"}");
}

TEST(HttpRequest, Built) {
    std::string expect = 
        "GET /some_resource/indentifier HTTP/1.1\n"
        "Host: example.com\n"
        "User-Agent: curl/7.81.0\n"
        "Accept: */*\n"
        "Content-Length: 4\n"
        "Content-Type: application/x-www-form-urlencoded\n\n";
        "{\"somedata\":\"and_its_value\"}";
    srv::HttpRequest parsed = srv::HttpRequest::Parse(expect);
    
    std::string rebuilted = parsed.Build();
    srv::HttpRequest parsed2 = srv::HttpRequest::Parse(rebuilted);

    auto headers = parsed.GetHeaders();   
    auto headers2 = parsed2.GetHeaders();   

    EXPECT_EQ(parsed.GetHttpVersion(), parsed2.GetHttpVersion());
    EXPECT_EQ(parsed.GetHttpMethod(), parsed2.GetHttpMethod());
    EXPECT_EQ(parsed.GetRequestedResource(), parsed2.GetRequestedResource());

    EXPECT_EQ(headers["Host"], headers2["Host"]);
    EXPECT_EQ(headers["User-Agent"], headers2["User-Agent"]);
    EXPECT_EQ(headers["Accept"], headers2["Accept"]);
    EXPECT_EQ(headers["Content-Length"], headers2["Content-Length"]);
    EXPECT_EQ(headers["Content-Type"], headers2["Content-Type"]);
    EXPECT_EQ(parsed.GetBody(), parsed2.GetBody());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
