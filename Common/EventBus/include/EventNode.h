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
    EventNode(int listenPort) : listenPort(listenPort) {}

    // 注册事件回调
    void onEvent(const std::string& type, std::function<void(const Event&)> cb) {
        callbacks[type] = cb;
    }

    // 广播事件到所有连接
    void broadcast(const Event& e);

    // 连接其他节点
    void connectTo(const std::string& host, int port);

    // 启动 libev 事件循环
    void run();

private:
    int listenPort;
    int listenFd;
    struct ev_loop* loop;
    std::vector<int> peers;
    std::unordered_map<std::string, std::function<void(const Event&)>> callbacks;

    static void accept_cb(EV_P_ ev_io* w, int revents);
    static void read_cb(EV_P_ ev_io* w, int revents);
};
