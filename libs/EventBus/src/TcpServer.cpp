#include "TcpServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

TcpServer::TcpServer(int port, MessageHandler handler) 
    : port_(port), serverSocket_(-1), messageHandler_(handler), running_(false) {}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        std::cerr << "Failed to create server socket" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(serverSocket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind server socket" << std::endl;
        close(serverSocket_);
        return;
    }

    if (listen(serverSocket_, 10) < 0) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        close(serverSocket_);
        return;
    }

    running_ = true;
    acceptThread_ = std::thread(&TcpServer::acceptLoop, this);
}

void TcpServer::stop() {
    running_ = false;
    if (serverSocket_ >= 0) {
        close(serverSocket_);
        serverSocket_ = -1;
    }
    
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    
    for (auto& thread : clientThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    clientThreads_.clear();
}

void TcpServer::acceptLoop() {
    while (running_) {
        sockaddr_in clientAddress{};
        socklen_t clientLen = sizeof(clientAddress);
        
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddress, &clientLen);
        if (clientSocket >= 0) {
            clientThreads_.emplace_back(&TcpServer::handleClient, this, clientSocket);
        }
    }
}

void TcpServer::handleClient(int clientSocket) {
    while (running_) {
        uint32_t messageSize;
        ssize_t bytesRead = recv(clientSocket, &messageSize, sizeof(messageSize), MSG_WAITALL);
        if (bytesRead <= 0) break;

        messageSize = ntohl(messageSize);
        std::vector<char> buffer(messageSize);
        bytesRead = recv(clientSocket, buffer.data(), messageSize, MSG_WAITALL);
        if (bytesRead <= 0) break;

        EventMessage message;
        if (message.ParseFromArray(buffer.data(), messageSize)) {
            messageHandler_(message);
        }
    }
    close(clientSocket);
}
