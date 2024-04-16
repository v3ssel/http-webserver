CXX = g++
CXXFLAGS = -std=c++17
PROJECTDIR = $(CURDIR)

SERVER_SOURCES = \
	$(PROJECTDIR)/server_src/main.cpp \
	$(PROJECTDIR)/server_src/server.cpp \
	$(PROJECTDIR)/server_src/connection_logger.cpp

CLIENT_SOURCES = \
	$(PROJECTDIR)/client_src/main.cpp \

HTTP_SOURCES = \
	$(PROJECTDIR)/server_src/http_message.cpp \
	$(PROJECTDIR)/server_src/http_request.cpp \
	$(PROJECTDIR)/server_src/http_response.cpp

BINDER_SOURCES = \
	$(PROJECTDIR)/server_src/resource_binder.cpp

.PHONY: server client http_tests binder_tests

server:
	$(CXX) $(CXXFLAGS) $(SERVER_SOURCES) -o server

client:
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) -o client

http_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/http_message_tests.cpp $(HTTP_SOURCES) -lgtest -o http_tests

binder_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/resource_binder_tests.cpp $(BINDER_SOURCES) $(HTTP_SOURCES) -lgtest -o binder_tests
