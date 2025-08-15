#include "EventNode.h"
#include <cstring>

EventNode::EventNode(int listenPort) : listenPort(listenPort) {}

void EventNode::onEvent(const std::string& type, std::function<void(const Event&)> cb) {
    callbacks[type] = cb;
}

void EventNode::onAnyEvent(std::function<void(const Event&)> cb) {
    anyCallbacks.push_back(cb);
}

void EventNode::broadcast(const Event& e) {
    std::string msg = e.type + ":" + e.data;
    for (int fd : peers) send(fd, msg.c_str(), msg.size(), 0);
}

void EventNode::connectTo(const std::string& host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(sock); return;
    }
    peers.push_back(sock);
}

void EventNode::run() {
    loop = ev_default_loop(0);
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1; setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listenPort);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listenFd, (sockaddr*)&addr, sizeof(addr));
    listen(listenFd, 10);

    ev_io* w_accept = new ev_io;
    ev_io_init(w_accept, EventNode::accept_cb, listenFd, EV_READ);
    w_accept->data = this;
    ev_io_start(loop, w_accept);

    ev_run(loop, 0);
}

void EventNode::accept_cb(EV_P_ ev_io* w, int revents) {
    EventNode* self = static_cast<EventNode*>(w->data);
    int clientFd = accept(self->listenFd, nullptr, nullptr);
    if (clientFd >= 0) {
        self->peers.push_back(clientFd);
        ev_io* w_read = new ev_io;
        ev_io_init(w_read, EventNode::read_cb, clientFd, EV_READ);
        w_read->data = self;
        ev_io_start(self->loop, w_read);
    }
}

void EventNode::read_cb(EV_P_ ev_io* w, int revents) {
    EventNode* self = static_cast<EventNode*>(w->data);
    char buf[1024]{0};
    int n = recv(((ev_io*)w)->fd, buf, sizeof(buf)-1, 0);
    if (n <= 0) { close(((ev_io*)w)->fd); ev_io_stop(self->loop, (ev_io*)w); return; }

    std::string msg(buf, n);
    auto pos = msg.find(':');
    if (pos != std::string::npos) {
        Event e{msg.substr(0,pos), msg.substr(pos+1)};
        auto it = self->callbacks.find(e.type);
        if (it != self->callbacks.end()) it->second(e);
        for (auto& cb : self->anyCallbacks) cb(e);
    }
}
