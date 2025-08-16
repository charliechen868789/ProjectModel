#pragma once
#include <string>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <future>
#include "event_message.pb.h"

class TcpClient {
public:
    using MessageHandler = std::function<void(const EventMessage&)>;
    using ConnectionStateHandler = std::function<void(bool)>;
    
    TcpClient(const std::string& host, int port, MessageHandler messageHandler,
              ConnectionStateHandler connectionHandler = nullptr);
    ~TcpClient();
    
    bool connect();
    void disconnect();
    void sendMessage(const EventMessage& message);
    bool sendMessageAsync(const EventMessage& message, std::chrono::milliseconds timeout);
    
    bool isConnected() const { return connected_.load(); }
    std::string getEndpoint() const { return host_ + ":" + std::to_string(port_); }

private:
    void receiveLoop();
    void sendLoop();
    bool connectSocket();
    void closeSocket();
    void notifyConnectionState(bool connected);
    
    std::string host_;
    int port_;
    int socket_;
    MessageHandler messageHandler_;
    ConnectionStateHandler connectionHandler_;
    
    std::thread receiveThread_;
    std::thread sendThread_;
    std::atomic<bool> connected_;
    std::atomic<bool> shouldStop_;
    
    // Send queue
    std::queue<EventMessage> sendQueue_;
    std::mutex sendMutex_;
    std::condition_variable sendCondition_;
    
    // Connection management
    mutable std::mutex connectionMutex_;
    static constexpr int CONNECT_TIMEOUT_MS = 3000;
    static constexpr int SEND_TIMEOUT_MS = 1000;
};

