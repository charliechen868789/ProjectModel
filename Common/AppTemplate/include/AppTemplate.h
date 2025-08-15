#pragma once
#include "EventNode.h"
#include <string>

class AppTemplate {
public:
    AppTemplate(const std::string& name, int port);

    void addConnection(const std::string& host, int port);
    void run();

protected:
    std::string appName;
    EventNode node;

    virtual void handleEvent(const Event& e) = 0;
};
