#include "AppTemplate.h"
#include <iostream>

class WebApp : public AppTemplate {
public:
    WebApp() : AppTemplate("WebApp", 5003) {}

protected:
    void handleEvent(const Event& e) override {
        if (e.type == "processed_data") {
            std::cout << "[WebApp] 更新网页: " << e.data << "\n";
        } else if (e.type == "alert") {
            std::cout << "[WebApp] ⚠️ 警告: " << e.data << "\n";
        }
    }
};

int main() {
    WebApp web;
    web.addConnection("127.0.0.1", 5000); // VirtualSensor
    web.addConnection("127.0.0.1", 5001); // Algorithm
    web.addConnection("127.0.0.1", 5002); // GUI

    web.run();
}
