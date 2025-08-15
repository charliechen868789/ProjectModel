#include "AppTemplate.h"
#include <iostream>
#include <random>

AppTemplate::AppTemplate(const std::string& appName, int port) 
    : appName_(appName), running_(false) {
    
    // 如果端口为0，随机分配
    if (port == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(20000, 30000);
        port = dis(gen);
    }
    
    eventBus_ = std::make_unique<EventBus>(port);
    std::cout << "[" << appName_ << "] Initialized on port " << port << std::endl;
}

AppTemplate::~AppTemplate() {
    stop();
}

void AppTemplate::postEvent(const std::string& eventType, const std::string& data) {
    eventBus_->postEvent(eventType, data);
}

void AppTemplate::broadcast(const std::string& eventType, const std::string& data) {
    eventBus_->broadcast(eventType, data);
}

void AppTemplate::registerHandler(const std::string& eventType, EventBus::EventHandler handler) {
    eventBus_->registerHandler(eventType, handler);
}

void AppTemplate::connectToPeer(const std::string& host, int port) {
    eventBus_->connectToPeer(host, port);
}

void AppTemplate::start() {
    running_ = true;
    eventBus_->start();
    
    std::cout << "[" << appName_ << "] Starting application..." << std::endl;
    initialize();
    run();
}

void AppTemplate::stop() {
    if (running_) {
        running_ = false;
        cleanup();
        eventBus_->stop();
        std::cout << "[" << appName_ << "] Application stopped" << std::endl;
    }
}