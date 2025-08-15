#include "GUI.h"
#include <csignal>
#include <memory>

std::unique_ptr<GUI> guiApp;

void signalHandler(int signal) {
    if (guiApp) {
        guiApp->stop();
    }
    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    guiApp = std::make_unique<GUI>();
    guiApp->start();
    
    return 0;
}