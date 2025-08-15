#include "TcpClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

TcpClient::TcpClient(const std::string& host, int port, MessageHandler handler)
    : host_(host), port_(port), socket_(-1), messageHandler_(handler), connected_(false) {}

TcpClient::~TcpClient() {
    disconnect();
}

void TcpClient::connect() {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        std::cerr << "Failed to create client socket" << std::endl;
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    if (inet_pton(AF_INET, host_.c_str(), &address.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << host_ << std::endl;
        close(socket_);
        return;
    }

    if (::connect(socket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to connect to " << host_ << ":" << port_ << std::endl;
        close(socket_);
        return;
    }

    connected_ = true;
    receiveThread_ = std::thread(&TcpClient::receiveLoop, this);
    std::cout << "Connected to " << host_ << ":" << port_ << std::endl;
}

void TcpClient::disconnect() {
    connected_ = false;
    if (socket_ >= 0) {
        close(socket_);
        socket_ = -1;
    }
    
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
}

void TcpClient::sendMessage(const EventMessage& message) {
    if (!connected_) return;

    std::string serialized = message.SerializeAsString();
    uint32_t messageSize = htonl(serialized.size());
    
    send(socket_, &messageSize, sizeof(messageSize), 0);
    send(socket_, serialized.data(), serialized.size(), 0);
}

void TcpClient::receiveLoop() {
    while (connected_) {
        uint32_t messageSize;
        ssize_t bytesRead = recv(socket_, &messageSize, sizeof(messageSize), MSG_WAITALL);
        if (bytesRead <= 0) break;

        messageSize = ntohl(messageSize);
        std::vector<char> buffer(messageSize);
        bytesRead = recv(socket_, buffer.data(), messageSize, MSG_WAITALL);
        if (bytesRead <= 0) break;

        EventMessage message;
        if (message.ParseFromArray(buffer.data(), messageSize)) {
            messageHandler_(message);
        }
    }
}