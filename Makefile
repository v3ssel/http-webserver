CXX = g++
CXXFLAGS = -std=c++17
PROJECTDIR = $(CURDIR)

CLIENT_SOURCES = \
	$(PROJECTDIR)/client_src/main.cpp

MAIN = \
	$(PROJECTDIR)/server_src/main.cpp 

TCP_SERVER_SOURCES = \
	$(PROJECTDIR)/server_src/tcp_server.cpp \
	$(PROJECTDIR)/server_src/connection_logger.cpp

HTTP_SOURCES = \
	$(PROJECTDIR)/server_src/http_server.cpp \
	$(PROJECTDIR)/server_src/http_message.cpp \
	$(PROJECTDIR)/server_src/http_request.cpp \
	$(PROJECTDIR)/server_src/http_response.cpp

BINDER_SOURCES = \
	$(PROJECTDIR)/server_src/resource_binder.cpp

.PHONY: http_server tcp_server client http_tests binder_tests

tcp_server:
	$(CXX) $(CXXFLAGS) $(MAIN) $(TCP_SERVER_SOURCES) -o server

http_server:
	$(CXX) $(CXXFLAGS) $(MAIN) $(TCP_SERVER_SOURCES) $(HTTP_SOURCES) $(BINDER_SOURCES) -o http_server

client:
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) -o client

http_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/http_message_tests.cpp $(HTTP_SOURCES) -lgtest -o http_tests

binder_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/resource_binder_tests.cpp $(BINDER_SOURCES) $(HTTP_SOURCES) -lgtest -o binder_tests
