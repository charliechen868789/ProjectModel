#include "AppTemplate.h"
#include <iostream>
#include <thread>
#include <chrono>

class VirtualSensorApp : public AppTemplate {
public:
    VirtualSensorApp() : AppTemplate("VirtualSensor", 5000) {}

    void startGenerating() {
        std::thread([this](){
            int count = 0;
            while(true){
                Event e{"raw_data", "data_" + std::to_string(count++)};
                node.broadcast(e);  // 子类内部访问 protected node
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();
    }

protected:
    void handleEvent(const Event& e) override {
        std::cout << "[VirtualSensor] 收到事件: " << e.type << " -> " << e.data << "\n";
    }
};

int main() {
    VirtualSensorApp sensor;
    sensor.addConnection("127.0.0.1", 5001); // Algorithm
    sensor.addConnection("127.0.0.1", 5002); // GUI
    sensor.addConnection("127.0.0.1", 5003); // WebApp

    sensor.startGenerating();
    sensor.run();
}
