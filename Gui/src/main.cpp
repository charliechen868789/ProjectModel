#include "EventNode.h"
#include <iostream>

int main() {
    EventNode node(5002);

    node.onEvent("processed_data", [](const Event& e){
        std::cout << "[GUI] 显示: " << e.data << "\n";
    });
    node.onEvent("alert", [](const Event& e){
        std::cout << "[GUI] ⚠️ 警告: " << e.data << "\n";
    });

    node.connectTo("127.0.0.1", 5000); // VirtualSensor
    node.connectTo("127.0.0.1", 5001); // Algorithm
    node.connectTo("127.0.0.1", 5003); // WebApp

    node.run();
}
