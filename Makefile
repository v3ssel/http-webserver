CXX = g++
CXXFLAGS = -std=c++11
PROJECTDIR = $(CURDIR)

SERVER_SOURCES = \
	$(PROJECTDIR)/server_src/main.cpp \
	$(PROJECTDIR)/server_src/server.cpp \
	$(PROJECTDIR)/server_src/connection_logger.cpp

CLIENT_SOURCES = \
	$(PROJECTDIR)/client_src/main.cpp \

.PHONY: server client

server:
	$(CXX) $(CXXFLAGS) $(SERVER_SOURCES) -o server

client:
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) -o client
