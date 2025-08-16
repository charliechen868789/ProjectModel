#include "VirtualSensor.h"
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

VirtualSensor::VirtualSensor() 
    : AppTemplate("VirtualSensor", 20001), generating_(false), sensorId_("SENSOR_001") {}

VirtualSensor::~VirtualSensor() {
    cleanup();
}

void VirtualSensor::initialize() {
    std::cout << "[VirtualSensor] Initializing sensor " << sensorId_ << std::endl;
    
    // 连接到其他应用（假设它们运行在默认端口）
    connectToPeer("127.0.0.1", 20002); // Algorithm
    // connectToPeer("localhost", 20003); // GUI
    // connectToPeer("localhost", 20004); // WebApp
}

void VirtualSensor::run() {
    generating_ = true;
    sensorThread_ = std::thread(&VirtualSensor::generateSensorData, this);
    
    std::cout << "[VirtualSensor] Started generating sensor data. Press Ctrl+C to stop." << std::endl;
    
    // 主线程等待
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void VirtualSensor::cleanup() {
    generating_ = false;
    if (sensorThread_.joinable()) {
        sensorThread_.join();
    }
}

void VirtualSensor::generateSensorData() {
    while (generating_) {
        SensorData data = createRandomSensorData();
        
        // 序列化数据
        std::string serializedData = data.SerializeAsString();
        
        // 广播传感器数据
        broadcast("sensor.data", serializedData);
        
        std::cout << "[VirtualSensor] Sent: T=" << data.temperature() 
                  << "°C, H=" << data.humidity() 
                  << "%, P=" << data.pressure() << "hPa" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

SensorData VirtualSensor::createRandomSensorData() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> tempDis(18.0, 28.0);
    static std::uniform_real_distribution<> humidityDis(30.0, 80.0);
    static std::uniform_real_distribution<> pressureDis(980.0, 1030.0);
    
    SensorData data;
    data.set_sensor_id(sensorId_);
    data.set_temperature(tempDis(gen));
    data.set_humidity(humidityDis(gen));
    data.set_pressure(pressureDis(gen));
    data.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    
    return data;
}