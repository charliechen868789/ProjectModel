#include "Algorithm.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>

Algorithm::Algorithm() : AppTemplate("Algorithm", 20002) {}

void Algorithm::initialize() {
    std::cout << "[Algorithm] Initializing algorithm processor" << std::endl;
    
    // 注册传感器数据处理器
    registerHandler("sensor.data", [this](const std::string& eventType, const std::string& data) {
        handleSensorData(eventType, data);
    });
    
    // 连接到传感器
    connectToPeer("localhost", 20001);
}

void Algorithm::run() {
    std::cout << "[Algorithm] Started processing sensor data. Press Ctrl+C to stop." << std::endl;
    
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Algorithm::cleanup() {
    std::cout << "[Algorithm] Cleaning up..." << std::endl;
}

void Algorithm::handleSensorData(const std::string& eventType, const std::string& data) {
    SensorData sensorData;
    if (!sensorData.ParseFromString(data)) {
        std::cerr << "[Algorithm] Failed to parse sensor data" << std::endl;
        return;
    }
    
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    // 添加到缓冲区
    sensorDataBuffer_.push_back(sensorData);
    if (sensorDataBuffer_.size() > BUFFER_SIZE) {
        sensorDataBuffer_.erase(sensorDataBuffer_.begin());
    }
    
    std::cout << "[Algorithm] Received sensor data from " << sensorData.sensor_id() 
              << " (buffer size: " << sensorDataBuffer_.size() << ")" << std::endl;
    
    // 如果有足够的数据，进行处理
    if (sensorDataBuffer_.size() >= 3) {
        AlgorithmResult result = processData();
        std::string serializedResult = result.SerializeAsString();
        broadcast("algorithm.result", serializedResult);
        
        std::cout << "[Algorithm] Processed data - Comfort Index: " << result.comfort_index() 
                  << ", Alert: " << result.alert_level() << std::endl;
    }
}

AlgorithmResult Algorithm::processData() {
    // 计算平均值
    double avgTemp = 0, avgHumidity = 0, avgPressure = 0;
    for (const auto& data : sensorDataBuffer_) {
        avgTemp += data.temperature();
        avgHumidity += data.humidity();
        avgPressure += data.pressure();
    }
    
    avgTemp /= sensorDataBuffer_.size();
    avgHumidity /= sensorDataBuffer_.size();
    avgPressure /= sensorDataBuffer_.size();
    
    double comfortIndex = calculateComfortIndex(avgTemp, avgHumidity, avgPressure);
    std::string alertLevel = determineAlertLevel(comfortIndex);
    std::string recommendation = generateRecommendation(comfortIndex, alertLevel);
    
    AlgorithmResult result;
    result.set_result_id("RESULT_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()));
    result.set_comfort_index(comfortIndex);
    result.set_alert_level(alertLevel);
    result.set_recommendation(recommendation);
    result.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    result.set_avg_temperature(avgTemp);
    result.set_avg_humidity(avgHumidity);
    result.set_avg_pressure(avgPressure);
    
    return result;
}

double Algorithm::calculateComfortIndex(double temp, double humidity, double pressure) {
    // 简单的舒适度计算算法
    double tempScore = 100 - std::abs(temp - 22.0) * 4; // 22°C为最佳温度
    double humidityScore = 100 - std::abs(humidity - 50.0) * 2; // 50%为最佳湿度
    double pressureScore = 100 - std::abs(pressure - 1013.25) * 0.1; // 标准大气压
    
    // 权重计算
    double comfortIndex = (tempScore * 0.5 + humidityScore * 0.3 + pressureScore * 0.2);
    return std::max(0.0, std::min(100.0, comfortIndex));
}

std::string Algorithm::determineAlertLevel(double comfortIndex) {
    if (comfortIndex >= 80) return "GOOD";
    if (comfortIndex >= 60) return "MODERATE";
    if (comfortIndex >= 40) return "POOR";
    return "CRITICAL";
}

std::string Algorithm::generateRecommendation(double comfortIndex, const std::string& alertLevel) {
    if (alertLevel == "GOOD") {
        return "Environment conditions are optimal";
    } else if (alertLevel == "MODERATE") {
        return "Consider adjusting temperature or humidity";
    } else if (alertLevel == "POOR") {
        return "Environmental conditions need attention";
    } else {
        return "Immediate action required - check HVAC system";
    }
}
