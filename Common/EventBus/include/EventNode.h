#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ev.h>
#include <unistd.h>
#include <iostream>

struct Event {
    std::string type;
    std::string data;
};

class EventNode {
public:
    EventNode(int listenPort);

    void onEvent(const std::string& type, std::function<void(const Event&)> cb);
    void onAnyEvent(std::function<void(const Event&)> cb);

    void broadcast(const Event& e);
    void connectTo(const std::string& host, int port);
    void run();

private:
    int listenPort;
    int listenFd;
    struct ev_loop* loop;
    std::vector<int> peers;
    std::unordered_map<std::string, std::function<void(const Event&)>> callbacks;
    std::vector<std::function<void(const Event&)>> anyCallbacks;

    static void accept_cb(EV_P_ ev_io* w, int revents);
    static void read_cb(EV_P_ ev_io* w, int revents);
};
