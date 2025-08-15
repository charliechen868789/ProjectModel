#include "HttpServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

HttpServer::HttpServer(int port, DataProvider dataProvider) 
    : port_(port), serverSocket_(-1), dataProvider_(dataProvider), running_(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        std::cerr << "[HttpServer] Failed to create socket" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(serverSocket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[HttpServer] Failed to bind to port " << port_ << std::endl;
        close(serverSocket_);
        return;
    }

    if (listen(serverSocket_, 10) < 0) {
        std::cerr << "[HttpServer] Failed to listen" << std::endl;
        close(serverSocket_);
        return;
    }

    running_ = true;
    serverThread_ = std::thread(&HttpServer::serverLoop, this);
    std::cout << "[HttpServer] HTTP server started on port " << port_ << std::endl;
}

void HttpServer::stop() {
    running_ = false;
    if (serverSocket_ >= 0) {
        close(serverSocket_);
        serverSocket_ = -1;
    }
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void HttpServer::serverLoop() {
    while (running_) {
        sockaddr_in clientAddress{};
        socklen_t clientLen = sizeof(clientAddress);
        
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddress, &clientLen);
        if (clientSocket >= 0) {
            std::thread(&HttpServer::handleClient, this, clientSocket).detach();
        }
    }
}

void HttpServer::handleClient(int clientSocket) {
    char buffer[4096];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string request(buffer);
        std::string response = handleRequest(request);
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    
    close(clientSocket);
}

std::string HttpServer::handleRequest(const std::string& request) {
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;
    
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html; charset=UTF-8\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n\r\n";
    
    if (path == "/api/data") {
        response.str("");
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "Connection: close\r\n\r\n";
        response << dataProvider_();
    } else {
        response << getWebPage();
    }
    
    return response.str();
}

std::string HttpServer::getWebPage() {
    return R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environmental Monitor</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #333; border-bottom: 2px solid #4CAF50; padding-bottom: 10px; }
        .data-section { margin: 20px 0; padding: 15px; border-left: 4px solid #4CAF50; background-color: #f9f9f9; }
        .data-row { margin: 10px 0; }
        .label { font-weight: bold; color: #555; }
        .value { color: #333; }
        .alert { padding: 10px; margin: 10px 0; border-radius: 5px; }
        .alert-GOOD { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .alert-MODERATE { background-color: #fff3cd; color: #856404; border: 1px solid #ffeaa7; }
        .alert-POOR { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .alert-CRITICAL { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; font-weight: bold; }
        .timestamp { font-size: 0.8em; color: #777; }
        .loading { text-align: center; color: #666; }
    </style>
</head>
<body>
    <div class="container">
        <h1 class="header">Environmental Monitor Dashboard</h1>
        
        <div id="content" class="loading">Loading data...</div>
        
        <div style="text-align: center; margin-top: 20px;">
            <button onclick="refreshData()" style="background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer;">Refresh</button>
        </div>
    </div>

    <script>
        function refreshData() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    const content = document.getElementById('content');
                    
                    if (data.sensorData) {
                        let html = '<div class="data-section">';
                        html += '<h3>Sensor Data</h3>';
                        html += '<div class="data-row"><span class="label">Temperature:</span> <span class="value">' + data.sensorData.temperature.toFixed(2) + ' °C</span></div>';
                        html += '<div class="data-row"><span class="label">Humidity:</span> <span class="value">' + data.sensorData.humidity.toFixed(2) + ' %</span></div>';
                        html += '<div class="data-row"><span class="label">Pressure:</span> <span class="value">' + data.sensorData.pressure.toFixed(2) + ' hPa</span></div>';
                        html += '<div class="timestamp">Last updated: ' + new Date(data.sensorData.timestamp).toLocaleString() + '</div>';
                        html += '</div>';
                        
                        if (data.algorithmResult) {
                            html += '<div class="alert alert-' + data.algorithmResult.alertLevel + '">';
                            html += '<h3>Analysis Result</h3>';
                            html += '<div class="data-row"><span class="label">Comfort Index:</span> <span class="value">' + data.algorithmResult.comfortIndex.toFixed(1) + '/100</span></div>';
                            html += '<div class="data-row"><span class="label">Alert Level:</span> <span class="value">' + data.algorithmResult.alertLevel + '</span></div>';
                            html += '<div class="data-row"><span class="label">Recommendation:</span> <span class="value">' + data.algorithmResult.recommendation + '</span></div>';
                            html += '<div class="timestamp">Analyzed: ' + new Date(data.algorithmResult.timestamp).toLocaleString() + '</div>';
                            html += '</div>';
                        }
                        
                        content.innerHTML = html;
                    } else {
                        content.innerHTML = '<div class="loading">No data available. Waiting for sensors...</div>';
                    }
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    document.getElementById('content').innerHTML = '<div class="alert alert-CRITICAL">Error loading data</div>';
                });
        }
        
        // Auto-refresh every 3 seconds
        setInterval(refreshData, 3000);
        
        // Initial load
        refreshData();
    </script>
</body>
</html>
    )HTML";
}