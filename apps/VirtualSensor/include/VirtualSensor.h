#pragma once
#include "AppTemplate.h"
#include "sensor_data.pb.h"
#include <thread>
#include <atomic>

class VirtualSensor : public AppTemplate {
public:
    VirtualSensor();
    ~VirtualSensor() override;

protected:
    void initialize() override;
    void run() override;
    void cleanup() override;

private:
    void generateSensorData();
    SensorData createRandomSensorData();

    std::thread sensorThread_;
    std::atomic<bool> generating_;
    std::string sensorId_;
};