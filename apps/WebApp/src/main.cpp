#include "WebApp.h"
#include <csignal>
#include <memory>

std::unique_ptr<webapp::WebApp> webApp;

void signalHandler(int signal) {
    if (webApp) {
        webApp->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "[WebApp] Starting URL request handler..." << std::endl;
    std::cout << "[WebApp] Web UI must be served by lighttpd on port 8080" << std::endl;
    std::cout << "[WebApp] Configure lighttpd to proxy /api requests to localhost:8081" << std::endl;
    
    webApp = std::make_unique<webapp::WebApp>();
    webApp->start();
    
    return 0;
}