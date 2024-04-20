#include "home_controller.h"

namespace srv {
HttpResponse HomeController::GetHome(const HttpRequest& request) {
    std::string response = "HTTP/1.1 200 OK\n"
                           "Content-Type: text/html\n"
                           "\n"
                           "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "\t<meta charset=\"UTF-8\">\n"
                           "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                           "\t<title>Welcome</title>\n"
                           "</head>\n"
                           "<body>\n"
                           "\t<h1>Hello!</h1>\n"
                           "\t<p>This is a simple HTTP server based on linux sockets.</p>\n"
                           "\t<p>For now it can:</p>\n"
                           "<ol>\n"
                           "\t<li>Start the server on specified address and port. Linux only.</li>\n"
                           "\t<li>Temporary stop the server.</li>\n"
                           "\t<li>Binding endpoints to methods with any HTTP methods.</li>\n"
                           "\t<li>Parse HTTP requests and responses from string and to string.</li>\n"
                           "\t<li>Building HTTP requests and responses with builder pattern.</li>\n"
                           "\t<li>Log connected users.</li>\n"
                           "</ol>\n"
                           "</body>\n"
                           "</html>\n";

    return srv::HttpResponse::Parse(response);
}

HttpResponse HomeController::GetHello(const HttpRequest& request) {
    std::string name = request.ContainsHeader("Name") ? request.GetHeaderValue("Name") : "World";

    std::string response = "HTTP/1.1 200 OK\n"
                           "Content-Type: text/html\n"
                           "\n"
                           "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "\t<meta charset=\"UTF-8\">\n"
                           "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                           "\t<title>Hello Page</title>\n"
                           "</head>\n"
                           "<body>\n"
                           "\t<h1>Hello, World!</h1>\n"
                           "\t<p>This is a named hello page.</p>\n"
                           "\t<p>Pass your name in header \"Name: &lt;name&gt;\".</p>\n"
                           "</body>\n"
                           "</html>\n";
    
    response.replace(response.find("Page"), 4, name);
    response.replace(response.find("World"), 5, name);

    return srv::HttpResponse::Parse(response);
}
}