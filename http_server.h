// Created by mars on 27/8/24

#ifndef MARS_HTTP_SERVER_H
#define MARS_HTTP_SERVER_H

#include "m_netlib/Net/EventLoop.h"
#include "m_netlib/Net/TcpServer.h"
#include "m_netlib/Base/Timestamp.h"

namespace mars {
namespace net {

class HttpRequest;
class HttpResponse;

class HttpServer {
public:
    typedef std::function<void(const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(EventLoop *loop, const InetAddress &listenAddr);

    EventLoop* getLoop() const { return m_server.getLoop(); }

    void setHttpCallback(const HttpCallback &cb) { m_httpCallback = cb; }

    void setThreadNum(int numThreads) { m_server.setThreadNum(numThreads); }

    void start();

private:
    void onConnection(const TcpConnectionPtr &conn);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, base::Timestamp receiveTime);

    void onRequest(const TcpConnectionPtr &conn, const HttpRequest &req);

    TcpServer m_server;
    HttpCallback m_httpCallback;
};

}
}

#endif //MARS_HTTP_SERVER_H
