#include "AppTemplate.h"
#include <iostream>
#include <thread>

class AlgorithmApp : public AppTemplate {
public:
    AlgorithmApp() : AppTemplate("Algorithm", 5001) {}

protected:
    void handleEvent(const Event& e) override {
        if (e.type == "raw_data") {
            std::cout << "[Algorithm] 处理数据: " << e.data << "\n";

            Event processed{"processed_data", e.data + "_processed"};
            node.broadcast(processed); // 子类内部访问 protected node
        }
    }
};

int main() {
    AlgorithmApp algo;
    algo.addConnection("127.0.0.1", 5000); // VirtualSensor
    algo.addConnection("127.0.0.1", 5002); // GUI
    algo.addConnection("127.0.0.1", 5003); // WebApp

    algo.run();
}
