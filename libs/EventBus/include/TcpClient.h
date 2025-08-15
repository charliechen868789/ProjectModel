#pragma once
#include <string>
#include <thread>
#include <functional>
#include "event_message.pb.h"

class TcpClient {
public:
    using MessageHandler = std::function<void(const EventMessage&)>;

    TcpClient(const std::string& host, int port, MessageHandler handler);
    ~TcpClient();

    void connect();
    void disconnect();
    void sendMessage(const EventMessage& message);

private:
    void receiveLoop();

    std::string host_;
    int port_;
    int socket_;
    MessageHandler messageHandler_;
    std::thread receiveThread_;
    bool connected_;
};