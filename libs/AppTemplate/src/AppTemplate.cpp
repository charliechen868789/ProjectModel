// AppTemplate.cpp - Fixed version
#include "AppTemplate.h"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>

// Static member definition
AppTemplate* AppTemplate::currentApp_ = nullptr;

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
    
    // Set up signal handling
    currentApp_ = this;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN); // Ignore broken pipe signals
}

AppTemplate::~AppTemplate() {
    stop();
    currentApp_ = nullptr;
}

void AppTemplate::postEvent(const std::string& eventType, const std::string& data) {
    if (eventBus_) {
        eventBus_->postEvent(eventType, data);
    }
}

void AppTemplate::broadcast(const std::string& eventType, const std::string& data) {
    if (eventBus_) {
        eventBus_->broadcast(eventType, data);
    }
}

void AppTemplate::registerHandler(const std::string& eventType, EventBus::EventHandler handler) {
    if (eventBus_) {
        eventBus_->registerHandler(eventType, handler);
    }
}

void AppTemplate::connectToPeer(const std::string& host, int port) {
    if (eventBus_) {
        // Add a small delay to ensure the target service is ready
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        eventBus_->connectToPeer(host, port);
    }
}

void AppTemplate::start() {
    if (running_.load()) {
        std::cout << "[" << appName_ << "] Already running" << std::endl;
        return;
    }
    
    running_.store(true);
    
    try {
        eventBus_->start();
        
        std::cout << "[" << appName_ << "] Starting application..." << std::endl;
        initialize();
        run();
    } catch (const std::exception& e) {
        std::cerr << "[" << appName_ << "] Error during startup: " << e.what() << std::endl;
        stop();
    }
}

void AppTemplate::stop() {
    if (!running_.load()) {
        return;
    }
    
    std::cout << "[" << appName_ << "] Stopping application..." << std::endl;
    running_.store(false);
    
    try {
        cleanup();
    } catch (const std::exception& e) {
        std::cerr << "[" << appName_ << "] Error during cleanup: " << e.what() << std::endl;
    }
    
    if (eventBus_) {
        eventBus_->stop();
    }
    
    std::cout << "[" << appName_ << "] Application stopped" << std::endl;
}

void AppTemplate::signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << std::endl;
    if (currentApp_) {
        currentApp_->stop();
    }
    exit(0);
}