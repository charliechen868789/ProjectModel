#include "GUI.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

GUI::GUI() : AppTemplate("GUI", 20003), hasData_(false) {}

void GUI::initialize() {
    std::cout << "[GUI] Initializing GUI application" << std::endl;
    
    // 注册事件处理器
    registerHandler("sensor.data", [this](const std::string& eventType, const std::string& data) {
        handleSensorData(eventType, data);
    });
    
    registerHandler("algorithm.result", [this](const std::string& eventType, const std::string& data) {
        handleAlgorithmResult(eventType, data);
    });
    
    // 连接到其他应用
    connectToPeer("localhost", 20001); // VirtualSensor
    connectToPeer("localhost", 20002); // Algorithm
}

void GUI::run() {
    std::cout << "[GUI] Starting GUI interface..." << std::endl;
    displayMenu();
    
    // 自动显示更新的数据
    std::thread displayThread([this]() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (hasData_) {
                system("clear"); // Linux/Mac, 使用 "cls" for Windows
                displayCurrentStatus();
                displayMenu();
            }
        }
    });
    
    processUserInput();
    
    if (displayThread.joinable()) {
        displayThread.join();
    }
}

void GUI::cleanup() {
    std::cout << "[GUI] Cleaning up GUI..." << std::endl;
}

void GUI::handleSensorData(const std::string& eventType, const std::string& data) {
    SensorData sensorData;
    if (!sensorData.ParseFromString(data)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    latestSensorData_ = sensorData;
    hasData_ = true;
}

void GUI::handleAlgorithmResult(const std::string& eventType, const std::string& data) {
    AlgorithmResult result;
    if (!result.ParseFromString(data)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    latestResult_ = result;
}

void GUI::displayCurrentStatus() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::cout << "========================================" << std::endl;
    std::cout << "        ENVIRONMENTAL MONITOR GUI       " << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (hasData_) {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Sensor Data:" << std::endl;
        std::cout << "  Temperature: " << latestSensorData_.temperature() << " °C" << std::endl;
        std::cout << "  Humidity:    " << latestSensorData_.humidity() << " %" << std::endl;
        std::cout << "  Pressure:    " << latestSensorData_.pressure() << " hPa" << std::endl;
        
        if (latestResult_.comfort_index()) {
            std::cout << "\nAlgorithm Results:" << std::endl;
            std::cout << "  Comfort Index: " << latestResult_.comfort_index() << "/100" << std::endl;
            std::cout << "  Alert Level:   " << latestResult_.alert_level() << std::endl;
            std::cout << "  Recommendation: " << latestResult_.recommendation() << std::endl;
        }
    } else {
        std::cout << "Waiting for sensor data..." << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
}

void GUI::displayMenu() {
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  r - Refresh display" << std::endl;
    std::cout << "  s - Show detailed status" << std::endl;
    std::cout << "  q - Quit" << std::endl;
    std::cout << "> ";
}

void GUI::processUserInput() {
    std::string input;
    while (running_ && std::getline(std::cin, input)) {
        if (input == "q" || input == "quit") {
            break;
        } else if (input == "r" || input == "refresh") {
            system("clear");
            displayCurrentStatus();
            displayMenu();
        } else if (input == "s" || input == "status") {
            displayCurrentStatus();
            displayMenu();
        } else {
            std::cout << "Unknown command. Try 'r', 's', or 'q'." << std::endl;
            std::cout << "> ";
        }
    }
}