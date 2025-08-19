
#pragma once
#include <thread>
#include <functional>
#include <string>
#include <map>

namespace webapp 
{
    class HttpServer {
    public:
        using RequestHandler = std::function<std::string(const std::string& method, const std::string& path, const std::string& body)>;

        HttpServer(int port, RequestHandler requestHandler);
        ~HttpServer();

        void start();
        void stop();

    private:
        void serverLoop();
        void handleClient(int clientSocket);
        std::string handleRequest(const std::string& request);
        std::string parseRequestBody(const std::string& request);
        std::string getHttpMethod(const std::string& request);
        std::string getRequestPath(const std::string& request);

        int port_;
        int serverSocket_;
        RequestHandler requestHandler_;
        std::thread serverThread_;
        bool running_;
    };
}