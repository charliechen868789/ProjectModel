#include "EventBus.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include <iostream>
#include <chrono>

EventBus::EventBus(int port) : port_(port), running_(false) {}

EventBus::~EventBus() {
    stop();
}

void EventBus::postEvent(const std::string& eventType, const std::string& data) {
    distributeEvent(eventType, data);
}

void EventBus::registerHandler(const std::string& eventType, EventHandler handler) {
    std::lock_guard<std::mutex> lock(handlersMutex_);
    handlers_[eventType].push_back(handler);
}

void EventBus::broadcast(const std::string& eventType, const std::string& data) {
    EventMessage message;
    message.set_event_type(eventType);
    message.set_data(data);
    message.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    message.set_source("local");

    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        client->sendMessage(message);
    }

    // 同时本地分发
    distributeEvent(eventType, data);
}

void EventBus::connectToPeer(const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    auto client = std::make_unique<TcpClient>(host, port, 
        [this](const EventMessage& msg) { handleMessage(msg); });
    client->connect();
    clients_.push_back(std::move(client));
}

void EventBus::start() {
    running_ = true;
    server_ = std::make_unique<TcpServer>(port_, 
        [this](const EventMessage& msg) { handleMessage(msg); });
    server_->start();
    std::cout << "EventBus started on port " << port_ << std::endl;
}

void EventBus::stop() {
    running_ = false;
    if (server_) {
        server_->stop();
    }
    
    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        client->disconnect();
    }
    clients_.clear();
}

void EventBus::handleMessage(const EventMessage& message) {
    distributeEvent(message.event_type(), message.data());
}

void EventBus::distributeEvent(const std::string& eventType, const std::string& data) {
    std::lock_guard<std::mutex> lock(handlersMutex_);
    auto it = handlers_.find(eventType);
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            handler(eventType, data);
        }
    }
}