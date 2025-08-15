#include "AppTemplate.h"

AppTemplate::AppTemplate(const std::string& name, int port)
    : appName(name), node(port) {}

void AppTemplate::addConnection(const std::string& host, int port) {
    node.connectTo(host, port);
}

void AppTemplate::run() {
    node.onAnyEvent([this](const Event& e){ handleEvent(e); });
    node.run();
}
