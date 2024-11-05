#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class HttpProxy {
public:
    HttpProxy(int port);
    void start();

private:
    int server_fd;
    int port;
    void handleClient(int client_socket);
    void forwardRequest(const std::string &method, const std::string &url, const std::string &headers, int client_socket);
    void handleConnect(int client_socket, int server_socket);
};