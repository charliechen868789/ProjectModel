#include "WebApp.h"
#include "HttpServer.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

WebApp::WebApp() : AppTemplate("WebApp", 20004), hasData_(false) {}

WebApp::~WebApp() {
    cleanup();
}

void WebApp::initialize() {
    std::cout << "[WebApp] Initializing web application" << std::endl;
    
    // 注册事件处理器
    registerHandler("sensor.data", [this](const std::string& eventType, const std::string& data) {
        handleSensorData(eventType, data);
    });
    
    registerHandler("algorithm.result", [this](const std::string& eventType, const std::string& data) {
        handleAlgorithmResult(eventType, data);
    });
    
    // 连接到其他应用
    connectToPeer("127.0.0.1", 20001); // VirtualSensor
    connectToPeer("localhost", 20002); // Algorithm
    
    // 启动HTTP服务器
    httpServer_ = std::make_unique<HttpServer>(8080, [this]() {
        return getCurrentDataJson();
    });
    httpServer_->start();
    
    std::cout << "[WebApp] Web interface available at http://localhost:8080" << std::endl;
}

void WebApp::run() {
    std::cout << "[WebApp] Web application running. Press Ctrl+C to stop." << std::endl;
    
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void WebApp::cleanup() {
    if (httpServer_) {
        httpServer_->stop();
    }
}

void WebApp::handleSensorData(const std::string& eventType, const std::string& data) {
    SensorData sensorData;
    if (!sensorData.ParseFromString(data)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    latestSensorData_ = sensorData;
    hasData_ = true;
}

void WebApp::handleAlgorithmResult(const std::string& eventType, const std::string& data) {
    AlgorithmResult result;
    if (!result.ParseFromString(data)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    latestResult_ = result;
}

std::string WebApp::getCurrentDataJson() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::ostringstream json;
    json << "{";
    
    if (hasData_) {
        json << std::fixed << std::setprecision(2);
        json << "\"sensorData\": {";
        json << "\"temperature\": " << latestSensorData_.temperature() << ",";
        json << "\"humidity\": " << latestSensorData_.humidity() << ",";
        json << "\"pressure\": " << latestSensorData_.pressure() << ",";
        json << "\"timestamp\": " << latestSensorData_.timestamp();
        json << "},";
        
        if (latestResult_.comfort_index() > 0) {
            json << "\"algorithmResult\": {";
            json << "\"comfortIndex\": " << latestResult_.comfort_index() << ",";
            json << "\"alertLevel\": \"" << latestResult_.alert_level() << "\",";
            json << "\"recommendation\": \"" << latestResult_.recommendation() << "\",";
            json << "\"timestamp\": " << latestResult_.timestamp();
            json << "}";
        } else {
            json << "\"algorithmResult\": null";
        }
    } else {
        json << "\"sensorData\": null,";
        json << "\"algorithmResult\": null";
    }
    
    json << "}";
    return json.str();
}