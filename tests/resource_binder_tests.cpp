#include <gtest/gtest.h>

#include "../server_src/resource_binder.h"

TEST(ResourceBinder, Bind) {
    srv::ResourceBinder binder;
    binder.Bind("/", "GET", [](const srv::HttpRequest& r) -> srv::HttpResponse {
        srv::HttpResponse response;

        response.SetStatusCodeMean("Ok")
               ->SetStatusCode(200)
               ->AddHeader("Custom", "HeaderValue")
               ->SetHttpVersion(r.GetHttpVersion())
               ->SetBody("Hello " + r.GetBody().substr(0, r.GetBody().length() - 1) + "!", "text/plain");

        return response;
    });

    srv::HttpRequest request;
    request.SetHttpMethod("GET")
          ->SetResource("/")
          ->SetHttpVersion(srv::HttpVersion::HTTP_1_1)
          ->SetBody("Joe", "text/plain");

    srv::HttpResponse response = binder.Execute(request);
    EXPECT_EQ(response.GetHttpVersion(), request.GetHttpVersion());
    EXPECT_EQ(response.GetBody(), "Hello Joe!\n");
    EXPECT_EQ(response.GetStatusCode(), 200);
    EXPECT_EQ(response.GetStatusCodeMean(), "Ok");
    EXPECT_EQ(response.GetHeaderValue("Custom"), "HeaderValue");
}

TEST(ResourceBinder, DefaultErrorHandler) {
    srv::ResourceBinder binder;

    srv::HttpRequest request;
    request.SetHttpMethod("GET")
          ->SetResource("/")
          ->SetHttpVersion(srv::HttpVersion::HTTP_1_1)
          ->SetBody("Joe", "text/plain");

    srv::HttpResponse response = binder.Execute(request);
    EXPECT_EQ(response.GetHttpVersion(), request.GetHttpVersion());
    EXPECT_EQ(response.GetBody(), "Default error handler invoked on [GET] \'/\' with HTTP error code 404.\n");
    EXPECT_EQ(response.GetStatusCode(), 404);
    EXPECT_EQ(response.GetStatusCodeMean(), "Not Found");
    EXPECT_EQ(response.GetHeaderValue("Content-Type"), "text/plain");

    binder.Bind("/", "POST", [](const srv::HttpRequest& r) -> srv::HttpResponse { 
        return *srv::HttpResponse().SetStatusCode(200)->SetStatusCodeMean("Ok");
    });
    response = binder.Execute(request);

    EXPECT_EQ(response.GetHttpVersion(), request.GetHttpVersion());
    EXPECT_EQ(response.GetBody(), "Default error handler invoked on [GET] \'/\' with HTTP error code 405.\n");
    EXPECT_EQ(response.GetStatusCode(), 405);
    EXPECT_EQ(response.GetStatusCodeMean(), "Method Not Allowed");
    EXPECT_EQ(response.GetHeaderValue("Content-Type"), "text/plain");
}

TEST(ResourceBinder, CustomErrorHandler) {
    srv::ResourceBinder binder;
    binder.SetErrorCodeHandler([](const srv::HttpRequest& r, int code) -> srv::HttpResponse {
        srv::HttpResponse response;
        response.SetHttpVersion(r.GetHttpVersion());
        
        if (code == 405) {
            response.SetBody("It's okay :)", "text/plain");
            return *response.SetStatusCode(200)->SetStatusCodeMean("Ok");
        }
        
        response.SetBody("It's not okay.", "text/plain");
        return *response.SetStatusCode(code)->SetStatusCodeMean("Very bad");
    });

    srv::HttpRequest request;
    request.SetHttpMethod("GET")
          ->SetResource("/")
          ->SetHttpVersion(srv::HttpVersion::HTTP_1_1)
          ->SetBody("Joe", "text/plain");

    binder.Bind("/", "POST", [](const srv::HttpRequest& r) -> srv::HttpResponse {
        srv::HttpResponse response;
        response.SetStatusCode(200)->SetStatusCodeMean("Ok")->SetBody("You are not welcome here!", "text/plain");
        return response;
    });

    srv::HttpResponse response = binder.Execute(request);

    EXPECT_EQ(response.GetHttpVersion(), request.GetHttpVersion());
    EXPECT_EQ(response.GetBody(), "It's okay :)\n");
    EXPECT_EQ(response.GetStatusCode(), 200);
    EXPECT_EQ(response.GetStatusCodeMean(), "Ok");
    EXPECT_EQ(response.GetHeaderValue("Content-Type"), "text/plain");

    binder.Unbind("/", "POST");
    response = binder.Execute(request);

    EXPECT_EQ(response.GetHttpVersion(), request.GetHttpVersion());
    EXPECT_EQ(response.GetBody(), "It's not okay.\n");
    EXPECT_EQ(response.GetStatusCode(), 404);
    EXPECT_EQ(response.GetStatusCodeMean(), "Very bad");
    EXPECT_EQ(response.GetHeaderValue("Content-Type"), "text/plain");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
