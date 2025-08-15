#pragma once
#include "EventBus.h"
#include <string>
#include <memory>

class AppTemplate {
public:
    AppTemplate(const std::string& appName, int port = 0);
    virtual ~AppTemplate();

    // 事件处理接口
    void postEvent(const std::string& eventType, const std::string& data);
    void broadcast(const std::string& eventType, const std::string& data);
    void registerHandler(const std::string& eventType, EventBus::EventHandler handler);

    // 连接其他应用
    void connectToPeer(const std::string& host, int port);

    // 应用生命周期
    virtual void initialize() = 0;
    virtual void run() = 0;
    virtual void cleanup() = 0;

    void start();
    void stop();

protected:
    std::string appName_;
    std::unique_ptr<EventBus> eventBus_;
    bool running_;
};