#pragma once
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "event_message.pb.h"

class TcpServer;
class TcpClient;

class EventBus {
public:
    using EventHandler = std::function<void(const std::string&, const std::string&)>;

    EventBus(int port = 12345);
    ~EventBus();

    // 本地事件处理
    void postEvent(const std::string& eventType, const std::string& data);
    void registerHandler(const std::string& eventType, EventHandler handler);

    // 跨进程广播
    void broadcast(const std::string& eventType, const std::string& data);
    void connectToPeer(const std::string& host, int port);

    void start();
    void stop();

private:
    void handleMessage(const EventMessage& message);
    void distributeEvent(const std::string& eventType, const std::string& data);

    int port_;
    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
    std::unique_ptr<TcpServer> server_;
    std::vector<std::unique_ptr<TcpClient>> clients_;
    std::mutex handlersMutex_;
    std::mutex clientsMutex_;
    bool running_;
};