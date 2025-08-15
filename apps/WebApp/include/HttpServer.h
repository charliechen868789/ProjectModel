#pragma once
#include <thread>
#include <functional>
#include <string>

class HttpServer {
public:
    using DataProvider = std::function<std::string()>;

    HttpServer(int port, DataProvider dataProvider);
    ~HttpServer();

    void start();
    void stop();

private:
    void serverLoop();
    void handleClient(int clientSocket);
    std::string handleRequest(const std::string& request);
    std::string getWebPage();

    int port_;
    int serverSocket_;
    DataProvider dataProvider_;
    std::thread serverThread_;
    bool running_;
};