#include <gtest/gtest.h>

#include "../http/http_request.h"
#include "../http/http_response.h"

TEST(HttpRequest, Parse) {
    std::string request = 
        "GET /some_resource/indentifier HTTP/1.1\n"
        "Host: example.com\n"
        "User-Agent: curl/7.81.0\n"
        "Accept: */*\n"
        "Content-Length: 1\n"
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
    EXPECT_EQ(headers["Content-Length"], "29");
    EXPECT_EQ(headers["Content-Type"], "application/x-www-form-urlencoded");
    EXPECT_EQ(parsed.GetBody(), "{\"somedata\":\"and_its_value\"}\n");
}

TEST(HttpRequest, Build) {
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

TEST(HttpResponse, Parse) {
    std::string response_str = 
        "HTTP/1.1 400 Bad Request\n"
        "Content-Type: text/html\n"
        "Content-Length: 349\n"
        "Date: Mon, 15 Apr 2024 19:58:19 GMT\n"
        "Server: ECSF (sed/58CC)\n"
        "\n";

    std::string response_body =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
        "\t\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
        "\t<head>\n"
        "\t\t<title>400 - Bad Request</title>\n"
        "\t</head>\n"
        "\t<body>\n"
        "\t\t<h1>400 - Bad Request</h1>\n"
        "\t</body>\n"
        "</html>\n";

    srv::HttpResponse parsed = srv::HttpResponse::Parse(response_str + response_body);

    EXPECT_EQ(parsed.GetHttpVersion(), srv::HttpVersion::HTTP_1_1);
    EXPECT_EQ(parsed.GetStatusCode(), 400);
    EXPECT_EQ(parsed.GetStatusCodeMean(), "Bad Request");

    auto headers = parsed.GetHeaders();
    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "341");
    EXPECT_EQ(headers["Date"], "Mon, 15 Apr 2024 19:58:19 GMT");
    EXPECT_EQ(headers["Server"], "ECSF (sed/58CC)");
    EXPECT_EQ(parsed.GetBody(), response_body);
}

TEST(HttpResponse, Build) {
    std::string response_str = 
        "HTTP/1.1 400 Bad Request\n"
        "Content-Type: text/html\n"
        "Content-Length: 349\n"
        "Date: Mon, 15 Apr 2024 19:58:19 GMT\n"
        "Server: ECSF (sed/58CC)\n"
        "\n";

    std::string response_body =
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
        "\t\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
        "\t<head>\n"
        "\t\t<title>400 - Bad Request</title>\n"
        "\t</head>\n"
        "\t<body>\n"
        "\t\t<h1>400 - Bad Request</h1>\n"
        "\t</body>\n"
        "</html>";

    srv::HttpResponse parsed = srv::HttpResponse::Parse(response_str + response_body);

    std::string built = parsed.Build();
    srv::HttpResponse parsed2 = srv::HttpResponse::Parse(built);

    EXPECT_EQ(parsed.GetHttpVersion(), parsed2.GetHttpVersion());
    EXPECT_EQ(parsed.GetStatusCode(), parsed2.GetStatusCode());
    EXPECT_EQ(parsed.GetStatusCodeMean(), parsed2.GetStatusCodeMean());

    auto headers = parsed.GetHeaders();
    auto headers2 = parsed2.GetHeaders();

    EXPECT_EQ(headers["Content-Type"], headers2["Content-Type"]);
    EXPECT_EQ(headers["Content-Length"], headers2["Content-Length"]);
    EXPECT_EQ(headers["Date"], headers2["Date"]);
    EXPECT_EQ(headers["Server"], headers2["Server"]);
    EXPECT_EQ(parsed.GetBody(), parsed2.GetBody());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
