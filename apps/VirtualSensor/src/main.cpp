#include "VirtualSensor.h"
#include <csignal>
#include <memory>

std::unique_ptr<VirtualSensor> sensorApp;

void signalHandler(int signal) {
    if (sensorApp) {
        sensorApp->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    sensorApp = std::make_unique<VirtualSensor>();
    sensorApp->start();
    
    return 0;
}