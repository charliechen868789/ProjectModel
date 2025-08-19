// VirtualSensor.cpp - Fixed version
#include "VirtualSensor.h"
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

namespace sensor
{
    VirtualSensor::VirtualSensor() 
        : AppTemplate("VirtualSensor", 20001), generating_(false), sensorId_("SENSOR_001") {}

    VirtualSensor::~VirtualSensor() {
        cleanup();
    }

    void VirtualSensor::initialize() {
        std::cout << "[VirtualSensor] Initializing sensor " << sensorId_ << std::endl;
        
        // 连接到其他应用（假设它们运行在默认端口）
        // Add delay and retry logic for connections
        std::thread connectionThread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            connectToPeer("127.0.0.1", 20002); // Algorithm
            connectToPeer("127.0.0.1", 20005); // WebApp
            // connectToPeer("127.0.0.1", 20003); // GUI
        });
        connectionThread.detach();
    }

    void VirtualSensor::run() {
        generating_.store(true);
        sensorThread_ = std::thread(&VirtualSensor::generateSensorData, this);
        
        std::cout << "[VirtualSensor] Started generating sensor data. Press Ctrl+C to stop." << std::endl;
        
        // 主线程等待
        while (isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void VirtualSensor::cleanup() {
        std::cout << "[VirtualSensor] Cleaning up..." << std::endl;
        generating_.store(false);
        
        if (sensorThread_.joinable()) {
            sensorThread_.join();
        }
    }

    void VirtualSensor::generateSensorData() {
        while (generating_.load() && isRunning()) {
            try {
                SensorData data = createRandomSensorData();
                
                // 序列化数据
                std::string serializedData = data.SerializeAsString();
                
                // 广播传感器数据 - using try-catch to handle connection issues
                try {
                    broadcast("sensor.data", serializedData);
                    
                    std::cout << "[VirtualSensor] Sent: T=" << data.temperature() 
                            << "°C, H=" << data.humidity() 
                            << "%, P=" << data.pressure() << "hPa" << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[VirtualSensor] Error broadcasting data: " << e.what() << std::endl;
                    // Continue running even if broadcast fails
                }
                
                // Sleep with interruption check
                for (int i = 0; i < 20 && generating_.load() && isRunning(); ++i) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                
            } catch (const std::exception& e) {
                std::cerr << "[VirtualSensor] Error in data generation: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        std::cout << "[VirtualSensor] Data generation stopped" << std::endl;
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
}