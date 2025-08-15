#pragma once
#include "AppTemplate.h"
#include "algorithm_result.pb.h"
#include "sensor_data.pb.h"
#include <vector>
#include <mutex>

class Algorithm : public AppTemplate {
public:
    Algorithm();
    ~Algorithm() override = default;

protected:
    void initialize() override;
    void run() override;
    void cleanup() override;

private:
    void handleSensorData(const std::string& eventType, const std::string& data);
    AlgorithmResult processData();
    double calculateComfortIndex(double temp, double humidity, double pressure);
    std::string determineAlertLevel(double comfortIndex);
    std::string generateRecommendation(double comfortIndex, const std::string& alertLevel);

    std::vector<SensorData> sensorDataBuffer_;
    std::mutex bufferMutex_;
    static constexpr size_t BUFFER_SIZE = 5;
};