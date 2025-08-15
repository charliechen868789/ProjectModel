#pragma once
#include "AppTemplate.h"
#include "algorithm_result.pb.h"
#include "sensor_data.pb.h"
#include <mutex>

class GUI : public AppTemplate {
public:
    GUI();
    ~GUI() override = default;

protected:
    void initialize() override;
    void run() override;
    void cleanup() override;

private:
    void handleSensorData(const std::string& eventType, const std::string& data);
    void handleAlgorithmResult(const std::string& eventType, const std::string& data);
    void displayCurrentStatus();
    void displayMenu();
    void processUserInput();

    SensorData latestSensorData_;
    AlgorithmResult latestResult_;
    std::mutex dataMutex_;
    bool hasData_;
};