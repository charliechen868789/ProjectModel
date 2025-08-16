// TcpServer.h
#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include "event_message.pb.h"

class TcpServer {
public:
    using MessageHandler = std::function<void(const EventMessage&)>;
    using ClientConnectionHandler = std::function<void(const std::string&, bool)>;

    TcpServer(int port, MessageHandler messageHandler,
              ClientConnectionHandler connectionHandler = nullptr);
    ~TcpServer();

    void start();
    void stop();
    
    bool isRunning() const { return running_.load(); }
    size_t getConnectedClientsCount() const;
    std::vector<std::string> getConnectedClients() const;

private:
    void acceptLoop();
    void handleClient(int clientSocket, const std::string& clientEndpoint);
    void cleanupFinishedThreads();
    std::string getClientEndpoint(int socket) const;
    
    int port_;
    int serverSocket_;
    MessageHandler messageHandler_;
    ClientConnectionHandler connectionHandler_;
    
    std::thread acceptThread_;
    std::vector<std::thread> clientThreads_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldStop_;
    
    // Client management
    mutable std::mutex clientsMutex_;
    std::unordered_set<std::string> connectedClients_;
    
    // Thread cleanup
    std::thread cleanupThread_;
    std::mutex threadsMutex_;
};
