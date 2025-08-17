// EventBus.cpp - Fixed version
#include "EventBus.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include <iostream>
#include <chrono>
#include <thread>

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
    
    // Send to connected clients with error handling
    for (auto it = clients_.begin(); it != clients_.end();) {
        try {
            if ((*it)->isConnected()) {
                (*it)->sendMessage(message);
                ++it;
            } else {
                // Remove disconnected clients
                std::cout << "Removing disconnected client: " << (*it)->getEndpoint() << std::endl;
                it = clients_.erase(it);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error sending message to " << (*it)->getEndpoint() 
                      << ": " << e.what() << std::endl;
            it = clients_.erase(it);
        }
    }

    // 同时本地分发
    distributeEvent(eventType, data);
}

void EventBus::connectToPeer(const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    
    // Check if already connected to this peer
    std::string endpoint = host + ":" + std::to_string(port);
    for (const auto& client : clients_) {
        if (client->getEndpoint() == endpoint && client->isConnected()) {
            std::cout << "Already connected to " << endpoint << std::endl;
            return;
        }
    }
    
    auto client = std::make_unique<TcpClient>(host, port, 
        [this](const EventMessage& msg) { handleMessage(msg); },
        [host, port](bool connected) {
            std::cout << "Connection to " << host << ":" << port 
                      << (connected ? " established" : " lost") << std::endl;
        });
    
    // Try to connect with retry
    bool connected = false;
    for (int attempts = 0; attempts < 3 && running_; ++attempts) {
        if (client->connect()) {
            connected = true;
            break;
        }
        std::cout << "Connection attempt " << (attempts + 1) << " to " 
                  << endpoint << " failed, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    if (connected) {
        clients_.push_back(std::move(client));
        std::cout << "Successfully connected to " << endpoint << std::endl;
    } else {
        std::cerr << "Failed to connect to " << endpoint << " after 3 attempts" << std::endl;
    }
}

void EventBus::start() {
    if (running_.load()) {
        return;
    }
    
    running_.store(true);
    server_ = std::make_unique<TcpServer>(port_, 
        [this](const EventMessage& msg) { handleMessage(msg); });
    server_->start();
    std::cout << "EventBus started on port " << port_ << std::endl;
}

void EventBus::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    
    if (server_) {
        server_->stop();
        server_.reset();
    }
    
    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        if (client) {
            client->disconnect();
        }
    }
    clients_.clear();
    
    std::cout << "EventBus stopped" << std::endl;
}

void EventBus::handleMessage(const EventMessage& message) {
    distributeEvent(message.event_type(), message.data());
}

void EventBus::distributeEvent(const std::string& eventType, const std::string& data) {
    std::lock_guard<std::mutex> lock(handlersMutex_);
    auto it = handlers_.find(eventType);
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            try {
                handler(eventType, data);
            } catch (const std::exception& e) {
                std::cerr << "Error in event handler for " << eventType 
                          << ": " << e.what() << std::endl;
            }
        }
    }
}