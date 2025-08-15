#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include "event_message.pb.h"

class EventBus;

class TcpServer {
public:
    using MessageHandler = std::function<void(const EventMessage&)>;

    TcpServer(int port, MessageHandler handler);
    ~TcpServer();

    void start();
    void stop();

private:
    void acceptLoop();
    void handleClient(int clientSocket);

    int port_;
    int serverSocket_;
    MessageHandler messageHandler_;
    std::thread acceptThread_;
    std::vector<std::thread> clientThreads_;
    bool running_;
};