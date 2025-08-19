#pragma once
#include "AppTemplate.h"
#include "algorithm_result.pb.h"
#include "sensor_data.pb.h"
#include <memory>
#include <mutex>

namespace webapp 
{

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
        
        // HTTP request handler
        std::string handleHttpRequest(const std::string& method, const std::string& path, const std::string& body);
        
        // API endpoint handlers
        std::string handleGetData();
        std::string handlePostConfig(const std::string& body);
        std::string handlePostCommand(const std::string& body);

    private:
        std::unique_ptr<HttpServer> httpServer_;
        SensorData latestSensorData_;
        AlgorithmResult latestResult_;
        mutable std::mutex dataMutex_;
        bool hasData_;
    };
}