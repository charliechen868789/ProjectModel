#include "Algorithm.h"
#include <csignal>
#include <memory>

std::unique_ptr<Algorithm> algorithmApp;

void signalHandler(int signal) {
    if (algorithmApp) {
        algorithmApp->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    algorithmApp = std::make_unique<Algorithm>();
    algorithmApp->start();
    
    return 0;
}