PROJECTDIR = $(CURDIR)
CXX = g++
CXXFLAGS = -std=c++17

MAIN = \
	$(PROJECTDIR)/main.cpp 

SERVER_SOURCES = \
	$(PROJECTDIR)/server_src/http_server.cpp \
	$(PROJECTDIR)/server_src/tcp_server.cpp \

HTTP_SOURCES = \
	$(PROJECTDIR)/http/http_message.cpp \
	$(PROJECTDIR)/http/http_request.cpp \
	$(PROJECTDIR)/http/http_response.cpp

LOGGER_SOURCES = \
	$(PROJECTDIR)/loggers/connection_logger.cpp

BINDER_SOURCES = \
	$(PROJECTDIR)/server_src/resource_binder.cpp

CONTROLLERS_SOURCES = \
	$(PROJECTDIR)/controllers/home_controller.cpp


.PHONY: http_server http_tests binder_tests

http_server:
	$(CXX) $(CXXFLAGS) $(MAIN) $(SERVER_SOURCES) $(HTTP_SOURCES) $(BINDER_SOURCES) $(LOGGER_SOURCES) $(CONTROLLERS_SOURCES) -o http_server

http_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/http_message_tests.cpp $(HTTP_SOURCES) -lgtest -o http_tests

binder_tests:
	$(CXX) $(CXXFLAGS) $(PROJECTDIR)/tests/resource_binder_tests.cpp $(BINDER_SOURCES) $(HTTP_SOURCES) -lgtest -o binder_tests
