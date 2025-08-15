#include "EventNode.h"
#include <iostream>

int main() {
    EventNode node(5001); // 监听端口5001

    node.onEvent("sensor_data", [&node](const Event& e){
        int val = std::stoi(e.data);
        std::cout << "[Algorithm] 收到数据: " << val << "\n";
        if (val > 80) {
            node.broadcast({"alert", "值超过80!"});
        }
        node.broadcast({"processed_data", "avg:" + std::to_string(val)});
    });

    node.connectTo("127.0.0.1", 5000); // VirtualSensor
    node.connectTo("127.0.0.1", 5002); // GUI
    node.connectTo("127.0.0.1", 5003); // WebApp

    node.run();
}
