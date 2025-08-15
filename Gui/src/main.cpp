#include "AppTemplate.h"
#include <iostream>

class GuiApp : public AppTemplate {
public:
    GuiApp() : AppTemplate("GUI", 5002) {}

protected:
    void handleEvent(const Event& e) override {
        if (e.type == "processed_data") {
            std::cout << "[GUI] 显示: " << e.data << "\n";
        } else if (e.type == "alert") {
            std::cout << "[GUI] ⚠️ 警告: " << e.data << "\n";
        }
    }
};

int main() {
    GuiApp gui;
    gui.addConnection("127.0.0.1", 5000); // VirtualSensor
    gui.addConnection("127.0.0.1", 5001); // Algorithm
    gui.addConnection("127.0.0.1", 5003); // WebApp

    gui.run();
}
