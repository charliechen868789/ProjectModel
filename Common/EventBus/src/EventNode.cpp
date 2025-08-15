#include "EventNode.h"
#include <fcntl.h>
#include <cstring>

EventNode* globalNode = nullptr; // libev 回调使用

void EventNode::broadcast(const Event& e) {
    std::string msg = e.type + "|" + e.data + "\n";
    for (int fd : peers) {
        send(fd, msg.c_str(), msg.size(), 0);
    }
}

void EventNode::connectTo(const std::string& host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
        peers.push_back(sock);
    } else {
        perror("connect failed");
    }
}

void EventNode::run() {
    globalNode = this;
    loop = ev_default_loop(0);

    // 创建监听 socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listenPort);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed"); return;
    }
    if (listen(listenFd, 10) < 0) {
        perror("listen failed"); return;
    }

    ev_io* acceptWatcher = new ev_io;
    ev_io_init(acceptWatcher, accept_cb, listenFd, EV_READ);
    ev_io_start(loop, acceptWatcher);

    ev_run(loop, 0);
}

void EventNode::accept_cb(EV_P_ ev_io* w, int revents) {
    int client_fd = accept(w->fd, nullptr, nullptr);
    if (client_fd >= 0) {
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        globalNode->peers.push_back(client_fd);

        ev_io* clientWatcher = new ev_io;
        ev_io_init(clientWatcher, read_cb, client_fd, EV_READ);
        ev_io_start(EV_A_ clientWatcher);
    }
}

void EventNode::read_cb(EV_P_ ev_io* w, int revents) {
    char buffer[1024];
    int n = recv(w->fd, buffer, sizeof(buffer)-1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        std::string line(buffer);
        auto sep = line.find('|');
        if (sep != std::string::npos) {
            Event e{line.substr(0, sep), line.substr(sep+1)};
            if (globalNode->callbacks.count(e.type)) {
                globalNode->callbacks[e.type](e);
            }
        }
    } else if (n == 0 || n < 0) {
        ev_io_stop(EV_A_ w);
        close(w->fd);
    }
}
