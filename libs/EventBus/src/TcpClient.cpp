// TcpClient.cpp - Fixed version with better error handling
#include "TcpClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <cstring>

TcpClient::TcpClient(const std::string& host, int port, MessageHandler messageHandler,
                     ConnectionStateHandler connectionHandler)
    : host_(host), port_(port), socket_(-1), messageHandler_(messageHandler),
      connectionHandler_(connectionHandler), connected_(false), shouldStop_(false) {
}

TcpClient::~TcpClient() {
    disconnect();
}

bool TcpClient::connect() {
    std::lock_guard<std::mutex> lock(connectionMutex_);
    
    if (connected_.load()) {
        return true;
    }
    
    if (!connectSocket()) {
        return false;
    }
    
    shouldStop_.store(false);
    connected_.store(true);
    
    // Start worker threads
    receiveThread_ = std::thread(&TcpClient::receiveLoop, this);
    sendThread_ = std::thread(&TcpClient::sendLoop, this);
    
    notifyConnectionState(true);
    std::cout << "Connected to " << getEndpoint() << std::endl;
    
    return true;
}

void TcpClient::disconnect() {
    if (!connected_.load()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(connectionMutex_);
    
    shouldStop_.store(true);
    connected_.store(false);
    
    // Wake up send thread
    sendCondition_.notify_all();
    
    closeSocket();
    
    // Wait for threads to finish
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
    if (sendThread_.joinable()) {
        sendThread_.join();
    }
    
    // Clear send queue
    {
        std::lock_guard<std::mutex> sendLock(sendMutex_);
        while (!sendQueue_.empty()) {
            sendQueue_.pop();
        }
    }
    
    notifyConnectionState(false);
    std::cout << "Disconnected from " << getEndpoint() << std::endl;
}

void TcpClient::sendMessage(const EventMessage& message) {
    std::cout << "Queued message, sendQueue size: " << sendQueue_.size() << std::endl;
    if (!connected_.load()) {
        std::cerr << "Warning: Trying to send message while not connected to " 
                  << getEndpoint() << std::endl;
        return; // Changed from throw to return to prevent crashes
    }
    
    {
        std::lock_guard<std::mutex> lock(sendMutex_);
        sendQueue_.push(message);
    }
    sendCondition_.notify_one();
}

bool TcpClient::sendMessageAsync(const EventMessage& message, std::chrono::milliseconds timeout) {
    if (!connected_.load()) {
        return false;
    }
    
    try {
        auto future = std::async(std::launch::async, [this, message]() -> bool {
            try {
                sendMessage(message);
                return true;
            } catch (...) {
                return false;
            }
        });
        
        return future.wait_for(timeout) == std::future_status::ready && future.get();
    } catch (...) {
        return false;
    }
}

bool TcpClient::connectSocket() {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set socket to non-blocking for connect timeout
    int flags = fcntl(socket_, F_GETFL, 0);
    fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    
    if (inet_pton(AF_INET, host_.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << host_ << std::endl;
        closeSocket();
        return false;
    }
    
    int result = ::connect(socket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    if (result < 0) {
        if (errno != EINPROGRESS) {
            std::cerr << "Connect failed immediately: " << strerror(errno) << std::endl;
            closeSocket();
            return false;
        }
        
        // Wait for connection with timeout
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(socket_, &writeSet);
        
        struct timeval timeout;
        timeout.tv_sec = CONNECT_TIMEOUT_MS / 1000;
        timeout.tv_usec = (CONNECT_TIMEOUT_MS % 1000) * 1000;
        
        result = select(socket_ + 1, nullptr, &writeSet, nullptr, &timeout);
        if (result <= 0) {
            std::cerr << "Connection timeout to " << getEndpoint() << std::endl;
            closeSocket();
            return false;
        }
        
        // Check if connection was successful
        int error;
        socklen_t len = sizeof(error);
        if (getsockopt(socket_, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
            std::cerr << "Connection failed to " << getEndpoint() << ": " 
                      << (error ? strerror(error) : "unknown error") << std::endl;
            closeSocket();
            return false;
        }
    }
    
    // Set socket back to blocking mode
    fcntl(socket_, F_SETFL, flags);
    
    // Set send/receive timeouts
    struct timeval tv;
    tv.tv_sec = SEND_TIMEOUT_MS / 1000;
    tv.tv_usec = (SEND_TIMEOUT_MS % 1000) * 1000;
    setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    return true;
}

void TcpClient::closeSocket() {
    if (socket_ >= 0) {
        shutdown(socket_, SHUT_RDWR); // Graceful shutdown
        close(socket_);
        socket_ = -1;
    }
}

void TcpClient::notifyConnectionState(bool connected) {
    if (connectionHandler_) {
        try {
            connectionHandler_(connected);
        } catch (const std::exception& e) {
            std::cerr << "Error in connection handler: " << e.what() << std::endl;
        }
    }
}

void TcpClient::receiveLoop() {
    while (connected_.load() && !shouldStop_.load()) {
        try {
            uint32_t messageSize;
            ssize_t received = recv(socket_, &messageSize, sizeof(messageSize), 0);
            
            if (received <= 0) {
                if (received == 0) {
                    std::cout << "Server " << getEndpoint() << " closed connection" << std::endl;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Timeout, continue
                    continue;
                } else {
                    std::cerr << "Receive error from " << getEndpoint() 
                              << ": " << strerror(errno) << std::endl;
                }
                break;
            }
            
            messageSize = ntohl(messageSize);
            if (messageSize > 10 * 1024 * 1024) { // 10MB limit
                std::cerr << "Message too large from " << getEndpoint() 
                          << ": " << messageSize << std::endl;
                break;
            }
            
            std::vector<char> buffer(messageSize);
            size_t totalReceived = 0;
            
            while (totalReceived < messageSize && connected_.load() && !shouldStop_.load()) {
                received = recv(socket_, buffer.data() + totalReceived, 
                              messageSize - totalReceived, 0);
                if (received <= 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue; // Timeout, retry
                    } else {
                        std::cerr << "Receive error from " << getEndpoint() 
                                  << ": " << strerror(errno) << std::endl;
                        goto exit_loop;
                    }
                } else {
                    totalReceived += received;
                }
            }
            
            EventMessage message;
            if (message.ParseFromArray(buffer.data(), messageSize)) {
                if (messageHandler_) {
                    try {
                        messageHandler_(message);
                    } catch (const std::exception& e) {
                        std::cerr << "Error in message handler: " << e.what() << std::endl;
                    }
                }
            } else {
                std::cerr << "Failed to parse message from " << getEndpoint() << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Exception in receive loop for " << getEndpoint() 
                      << ": " << e.what() << std::endl;
            break;
        }
    }
    
exit_loop:
    if (connected_.load()) {
        connected_.store(false);
        notifyConnectionState(false);
    }
}

void TcpClient::sendLoop() {
    std::cout << "sendLoop iteration, queue size: " << sendQueue_.size() << std::endl;
    while (!shouldStop_.load()) {
        EventMessage message;
        bool hasMessage = false;
        
        {
            std::unique_lock<std::mutex> lock(sendMutex_);
            sendCondition_.wait_for(lock, std::chrono::seconds(1), [this]() { 
                return !sendQueue_.empty() || shouldStop_.load(); 
            });
            
            if (!sendQueue_.empty()) {
                message = sendQueue_.front();
                sendQueue_.pop();
                hasMessage = true;
            }
        }
        
        if (hasMessage && connected_.load()) {
            try {
                std::string serialized = message.SerializeAsString();
                uint32_t messageSize = htonl(serialized.size());
                
                // Send size header
                ssize_t sent = send(socket_, &messageSize, sizeof(messageSize), MSG_NOSIGNAL);
                if (sent != sizeof(messageSize)) {
                    std::cerr << "Failed to send message size to " << getEndpoint() << std::endl;
                    connected_.store(false);
                    notifyConnectionState(false);
                    continue;
                }
                
                // Send message data
                size_t totalSent = 0;
                while (totalSent < serialized.size() && connected_.load()) {
                    sent = send(socket_, serialized.data() + totalSent, 
                              serialized.size() - totalSent, MSG_NOSIGNAL);
                    if (sent <= 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue; // Timeout, retry
                        } else {
                            std::cerr << "Send error to " << getEndpoint() 
                                      << ": " << strerror(errno) << std::endl;
                            connected_.store(false);
                            notifyConnectionState(false);
                            break;
                        }
                    } else {
                        totalSent += sent;
                    }
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Exception in send loop for " << getEndpoint() 
                          << ": " << e.what() << std::endl;
                connected_.store(false);
                notifyConnectionState(false);
            }
        }
    }
}