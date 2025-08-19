#include "WebApp.h"
#include "HttpServer.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

namespace webapp 
{

    WebApp::WebApp() : AppTemplate("WebApp", 20005), hasData_(false) {}

    WebApp::~WebApp()
    {
        cleanup();
    }

    void WebApp::initialize()
    {
        std::cout << "[WebApp] Initializing request handler (UI served by lighttpd)" << std::endl;
        
        // Register event handlers for inter-app communication
        registerHandler("sensor.data", [this](const std::string& eventType, const std::string& data) {
            handleSensorData(eventType, data);
        });
        
        registerHandler("algorithm.result", [this](const std::string& eventType, const std::string& data) {
            handleAlgorithmResult(eventType, data);
        });
        
        // Connect to other applications
        connectToPeer("127.0.0.1", 20001); // VirtualSensor
        connectToPeer("127.0.0.1", 20002); // Algorithm
        
        // Start HTTP server for URL request handling only
        httpServer_ = std::make_unique<HttpServer>(8081, 
            [this](const std::string& method, const std::string& path, const std::string& body) {
                return handleHttpRequest(method, path, body);
            }
        );
        httpServer_->start();
        
        std::cout << "[WebApp] URL request handler running on port 8081" << std::endl;
    }

    void WebApp::run() {
        std::cout << "[WebApp] Handling URL requests. Web UI served by lighttpd." << std::endl;
        
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
            std::cerr << "[WebApp] Failed to parse sensor data" << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(dataMutex_);
        latestSensorData_ = sensorData;
        hasData_ = true;
    }

    void WebApp::handleAlgorithmResult(const std::string& eventType, const std::string& data) {
        AlgorithmResult result;
        if (!result.ParseFromString(data)) {
            std::cerr << "[WebApp] Failed to parse algorithm result" << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(dataMutex_);
        latestResult_ = result;
    }

    std::string WebApp::handleHttpRequest(const std::string& method, const std::string& path, const std::string& body) {
        std::cout << "[WebApp] " << method << " " << path << std::endl;
        
        std::ostringstream response;
        
        // Route requests to appropriate handlers
        if (method == "GET" && path == "/api/data") {
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: application/json\r\n";
            response << "Access-Control-Allow-Origin: *\r\n";
            response << "Connection: close\r\n\r\n";
            response << handleGetData();
        }
        else if (method == "POST" && path == "/api/config") {
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: application/json\r\n";
            response << "Access-Control-Allow-Origin: *\r\n";
            response << "Connection: close\r\n\r\n";
            response << handlePostConfig(body);
        }
        else if (method == "POST" && path == "/api/command") {
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: application/json\r\n";
            response << "Access-Control-Allow-Origin: *\r\n";
            response << "Connection: close\r\n\r\n";
            response << handlePostCommand(body);
        }
        else {
            // Return 404 for unknown endpoints
            response << "HTTP/1.1 404 Not Found\r\n";
            response << "Content-Type: application/json\r\n";
            response << "Access-Control-Allow-Origin: *\r\n";
            response << "Connection: close\r\n\r\n";
            response << "{\"error\": \"Endpoint not found\"}";
        }
        
        return response.str();
    }

    std::string WebApp::handleGetData() {
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

    std::string WebApp::handlePostConfig(const std::string& body) {
        std::cout << "[WebApp] Configuration update: " << body << std::endl;
        
        // Parse and handle configuration updates
        // Example: {"refreshRate": 5000, "alertThreshold": 75}
        
        return "{\"status\": \"success\", \"message\": \"Configuration updated\"}";
    }

    std::string WebApp::handlePostCommand(const std::string& body) {
        std::cout << "[WebApp] Command received: " << body << std::endl;
        
        // Parse and handle commands
        // Example: {"action": "reset"}, {"action": "calibrate"}
        
        if (body.find("\"action\": \"reset\"") != std::string::npos) {
            std::lock_guard<std::mutex> lock(dataMutex_);
            hasData_ = false;
            return "{\"status\": \"success\", \"message\": \"Data reset\"}";
        }
        
        return "{\"status\": \"error\", \"message\": \"Unknown command\"}";
    }
}
