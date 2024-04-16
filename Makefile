CXX = g++
CXXFLAGS = -std=c++11
PROJECTDIR = $(CURDIR)

SERVER_SOURCES = \
	$(PROJECTDIR)/server_src/main.cpp \
	$(PROJECTDIR)/server_src/server.cpp \
	$(PROJECTDIR)/server_src/connection_logger.cpp

CLIENT_SOURCES = \
	$(PROJECTDIR)/client_src/main.cpp \

HTTP_SOURCES = \
	$(PROJECTDIR)/tests/http_message_tests.cpp \
	$(PROJECTDIR)/server_src/http_message.cpp \
	$(PROJECTDIR)/server_src/http_request.cpp \
	$(PROJECTDIR)/server_src/http_response.cpp \

.PHONY: server client tests

server:
	$(CXX) $(CXXFLAGS) $(SERVER_SOURCES) -o server

client:
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) -o client

http_tests:
	$(CXX) $(CXXFLAGS) $(HTTP_SOURCES) -lgtest -o http_test
