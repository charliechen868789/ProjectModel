#pragma once
#include "AppTemplate.h"
#include "algorithm_result.pb.h"
#include "sensor_data.pb.h"
#include <memory>
#include <mutex>

class HttpServer;

class WebApp : public AppTemplate {
public:
    WebApp();
    ~WebApp() override;

protected:
    void initialize() override;
    void run() override;
    void cleanup() override;

private:
    void handleSensorData(const std::string& eventType, const std::string& data);
    void handleAlgorithmResult(const std::string& eventType, const std::string& data);

public:
    std::string getCurrentDataJson() const;

private:
    std::unique_ptr<HttpServer> httpServer_;
    SensorData latestSensorData_;
    AlgorithmResult latestResult_;
    mutable std::mutex dataMutex_;
    bool hasData_;
};