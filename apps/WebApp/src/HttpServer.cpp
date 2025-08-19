#include "HttpServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

namespace webapp 
{

    HttpServer::HttpServer(int port, RequestHandler requestHandler) 
        : port_(port), serverSocket_(-1), requestHandler_(requestHandler), running_(false) {}

    HttpServer::~HttpServer() {
        stop();
    }

    void HttpServer::start() {
        serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket_ < 0) {
            std::cerr << "[HttpServer] Failed to create socket" << std::endl;
            return;
        }

        int opt = 1;
        setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);

        if (bind(serverSocket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "[HttpServer] Failed to bind to port " << port_ << std::endl;
            close(serverSocket_);
            return;
        }

        if (listen(serverSocket_, 10) < 0) {
            std::cerr << "[HttpServer] Failed to listen" << std::endl;
            close(serverSocket_);
            return;
        }

        running_ = true;
        serverThread_ = std::thread(&HttpServer::serverLoop, this);
        std::cout << "[HttpServer] Request handler started on port " << port_ << std::endl;
    }

    void HttpServer::stop() {
        running_ = false;
        if (serverSocket_ >= 0) {
            close(serverSocket_);
            serverSocket_ = -1;
        }
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
    }

    void HttpServer::serverLoop() {
        while (running_) {
            sockaddr_in clientAddress{};
            socklen_t clientLen = sizeof(clientAddress);
            
            int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddress, &clientLen);
            if (clientSocket >= 0) {
                std::thread(&HttpServer::handleClient, this, clientSocket).detach();
            }
        }
    }

    void HttpServer::handleClient(int clientSocket) {
        char buffer[8192];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            std::string response = handleRequest(request);
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        close(clientSocket);
    }

    std::string HttpServer::getHttpMethod(const std::string& request) {
        std::istringstream iss(request);
        std::string method;
        iss >> method;
        return method;
    }

    std::string HttpServer::getRequestPath(const std::string& request) {
        std::istringstream iss(request);
        std::string method, path;
        iss >> method >> path;
        return path;
    }

    std::string HttpServer::parseRequestBody(const std::string& request) {
        size_t bodyPos = request.find("\r\n\r\n");
        if (bodyPos != std::string::npos) {
            return request.substr(bodyPos + 4);
        }
        return "";
    }

    std::string HttpServer::handleRequest(const std::string& request) {
        std::string method = getHttpMethod(request);
        std::string path = getRequestPath(request);
        std::string body = parseRequestBody(request);
        
        // Handle CORS preflight
        if (method == "OPTIONS") {
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Access-Control-Allow-Origin: *\r\n";
            response << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
            response << "Access-Control-Allow-Headers: Content-Type\r\n";
            response << "Connection: close\r\n\r\n";
            return response.str();
        }
        
        // Delegate to the application's request handler
        return requestHandler_(method, path, body);
    }
}