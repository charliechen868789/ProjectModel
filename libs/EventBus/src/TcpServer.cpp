// TcpServer.cpp
#include <arpa/inet.h>
#include "TcpServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <cstring>
#include <algorithm>

TcpServer::TcpServer(int port, MessageHandler messageHandler,
                     ClientConnectionHandler connectionHandler)
    : port_(port), serverSocket_(-1), messageHandler_(messageHandler),
      connectionHandler_(connectionHandler), running_(false), shouldStop_(false) {
}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start() {
    if (running_.load()) {
        std::cout << "Server already running" << std::endl;
        return;
    }
    
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        throw std::runtime_error("Failed to create server socket: " + std::string(strerror(errno)));
    }
    
    // Allow socket reuse
    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);
    
    if (bind(serverSocket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket_);
        throw std::runtime_error("Failed to bind server socket: " + std::string(strerror(errno)));
    }
    
    if (listen(serverSocket_, 10) < 0) {
        close(serverSocket_);
        throw std::runtime_error("Failed to listen on server socket: " + std::string(strerror(errno)));
    }
    
    running_.store(true);
    shouldStop_.store(false);
    
    acceptThread_ = std::thread(&TcpServer::acceptLoop, this);
    cleanupThread_ = std::thread([this]() {
        while (running_.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cleanupFinishedThreads();
        }
    });
    
    std::cout << "Server started on port " << port_ << std::endl;
}

void TcpServer::stop() {
    if (!running_.load()) {
        return;
    }
    
    shouldStop_.store(true);
    running_.store(false);
    
    if (serverSocket_ >= 0) {
        close(serverSocket_);
        serverSocket_ = -1;
    }
    
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    
    if (cleanupThread_.joinable()) {
        cleanupThread_.join();
    }
    
    // Wait for all client threads to finish
    {
        std::lock_guard<std::mutex> lock(threadsMutex_);
        for (auto& thread : clientThreads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        clientThreads_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        connectedClients_.clear();
    }
    
    std::cout << "Server stopped" << std::endl;
}

size_t TcpServer::getConnectedClientsCount() const {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    return connectedClients_.size();
}

std::vector<std::string> TcpServer::getConnectedClients() const {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    return std::vector<std::string>(connectedClients_.begin(), connectedClients_.end());
}

void TcpServer::acceptLoop() {
    while (running_.load() && !shouldStop_.load()) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            if (running_.load() && errno != EINTR) {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        std::string clientEndpoint = getClientEndpoint(clientSocket);
        
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            connectedClients_.insert(clientEndpoint);
        }
        
        if (connectionHandler_) {
            connectionHandler_(clientEndpoint, true);
        }
        
        // Create thread to handle this client
        {
            std::lock_guard<std::mutex> lock(threadsMutex_);
            clientThreads_.emplace_back(&TcpServer::handleClient, this, clientSocket, clientEndpoint);
        }
        
        std::cout << "Client connected: " << clientEndpoint << std::endl;
    }
}

void TcpServer::handleClient(int clientSocket, const std::string& clientEndpoint) {
    while (running_.load() && !shouldStop_.load()) {
        try {
            uint32_t messageSize;
            ssize_t received = recv(clientSocket, &messageSize, sizeof(messageSize), 0);
            
            if (received <= 0) {
                if (received == 0) {
                    std::cout << "Client disconnected: " << clientEndpoint << std::endl;
                } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "Receive error from " << clientEndpoint << ": " << strerror(errno) << std::endl;
                }
                break;
            }
            
            messageSize = ntohl(messageSize);
            if (messageSize > 10 * 1024 * 1024) { // 10MB limit
                std::cerr << "Message too large from " << clientEndpoint << ": " << messageSize << std::endl;
                break;
            }
            
            std::vector<char> buffer(messageSize);
            size_t totalReceived = 0;
            
            while (totalReceived < messageSize && running_.load()) {
                received = recv(clientSocket, buffer.data() + totalReceived, 
                              messageSize - totalReceived, 0);
                if (received <= 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Receive error from " << clientEndpoint << ": " << strerror(errno) << std::endl;
                        goto client_disconnected;
                    }
                } else {
                    totalReceived += received;
                }
            }
            
            EventMessage message;
            if (message.ParseFromArray(buffer.data(), messageSize)) {
                if (messageHandler_) {
                    messageHandler_(message);
                }
            } else {
                std::cerr << "Failed to parse message from " << clientEndpoint << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Exception handling client " << clientEndpoint << ": " << e.what() << std::endl;
            break;
        }
    }
    
client_disconnected:
    close(clientSocket);
    
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        connectedClients_.erase(clientEndpoint);
    }
    
    if (connectionHandler_) {
        connectionHandler_(clientEndpoint, false);
    }
}

void TcpServer::cleanupFinishedThreads() {
    std::lock_guard<std::mutex> lock(threadsMutex_);
    
    clientThreads_.erase(
        std::remove_if(clientThreads_.begin(), clientThreads_.end(),
            [](std::thread& t) {
                if (t.joinable()) {
                    // Check if thread is still running (simple heuristic)
                    return false;
                } else {
                    return true;
                }
            }),
        clientThreads_.end());
}

std::string TcpServer::getClientEndpoint(int socket) const {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    
    if (getpeername(socket, (struct sockaddr*)&addr, &len) == 0) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
        return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
    }
    
    return "unknown:" + std::to_string(socket);
}