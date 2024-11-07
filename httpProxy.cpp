#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "httpProxy.h"
#include <netdb.h>

HttpProxy::HttpProxy(int port) : port(port)
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Serverul proxy ascultă pe portul " << port << "..." << std::endl;
}

    void HttpProxy::handleConnect(int client_socket, int server_socket) 
    {
        fd_set readfds;
        char buffer[4096];
        int bytes_received;
        while (true) 
        {
            FD_ZERO(&readfds);
            FD_SET(client_socket, &readfds);
            FD_SET(server_socket, &readfds);
            int max_sd = (client_socket > server_socket) ? client_socket : server_socket;
            int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

            if (activity < 0)
            {
                perror("select error");
                break;
            }
            if (FD_ISSET(client_socket, &readfds)) 
            {
                bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) 
                {
                    std::cout << "Conexiune închisă de client." << std::endl;
                    break;
                }
                send(server_socket, buffer, bytes_received, 0);
            }
            if (FD_ISSET(server_socket, &readfds))
            {
                bytes_received = recv(server_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) 
                {
                    std::cout << "Conexiune închisă de server." << std::endl;
                    break;
                }
                send(client_socket, buffer, bytes_received, 0); 
            }
        }
        close(server_socket);
        close(client_socket);
    }


    void HttpProxy::handleClient(int client_socket) 
    {
        char buffer[1024] = {0};
        int valread = read(client_socket, buffer, 1024);
        std::cout << "Cerere primită: " << buffer << std::endl;

        std::istringstream request_stream(buffer);
        std::string method;
        std::string url;
        std::string headers;    
        request_stream >> method >> url;
        std::string line;
        while (std::getline(request_stream, line) && line != "\r")
        {
            headers += line + "\r\n";
        }
        if (method == "CONNECT") 
        {
            std::string host;
            std::string port = "443"; 
            if (url.find(':') != std::string::npos) {
                size_t colon_pos = url.find(':');
                host = url.substr(0, colon_pos);
                port = url.substr(colon_pos + 1); 
            } 
                else 
            {
                host = url;
            }
            int server_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (server_socket < 0) {
                perror("socket failed");
                return;
            }
            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET; 
            hints.ai_socktype = SOCK_STREAM;
            if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
            {
                perror("getaddrinfo failed");
                close(server_socket);
                return;
            }

            struct sockaddr_in server_address;
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(stoi(port));
            memcpy(&server_address.sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
            freeaddrinfo(res); 
            if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
            {
                perror("connect failed");
                close(server_socket);
                return;
            }
            std::string response = "HTTP/1.1 200 Connection Established\r\n\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
            handleConnect(client_socket, server_socket);
        }
            else 
        {
        
            forwardRequest(method, url, headers, client_socket);
        }
    }

    void HttpProxy::forwardRequest(const std::string &method, const std::string &url, const std::string &headers, int client_socket)
    {
        std::string host;
        std::string port = "80"; //portu http
        std::string path;
        if (url.find("http://") == 0) {
            size_t start = 7; 
            size_t end = url.find('/', start);
            host = url.substr(start, end - start);
            path = url.substr(end);
        }
             else 
        {
            std::cerr << "Eroare: URL invalid." << std::endl;
            return; 
        }
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
         {
            perror("socket failed");
            return;
        }

        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; 
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) 
        {
            perror("getaddrinfo failed");
            close(server_socket);
            return;
        }
        struct sockaddr_in server_address;
        memcpy(&server_address, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res); 
        //ma conectez la server destinatie
        if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
        {
            perror("connect failed");
            close(server_socket);
            return;
        }
        // creez cerere
        std::string full_request = method + " " + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" +"Connection: close\r\n" + headers + "\r\n";

        // trimit cerere
        send(server_socket, full_request.c_str(), full_request.length(), 0);
        char response_buffer[4096] = {0};
        int bytes_received;
        while ((bytes_received = recv(server_socket, response_buffer, sizeof(response_buffer), 0)) > 0) {
            send(client_socket, response_buffer, bytes_received, 0);
        }
        close(server_socket); 
        close(client_socket); 
}


void HttpProxy::start() {
    while (true) {
      
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }
        handleClient(client_socket);
    }
}

