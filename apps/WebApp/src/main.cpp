#include "WebApp.h"
#include <csignal>
#include <memory>

std::unique_ptr<WebApp> webApp;

void signalHandler(int signal) {
    if (webApp) {
        webApp->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    webApp = std::make_unique<WebApp>();
    webApp->start();
    
    return 0;
}