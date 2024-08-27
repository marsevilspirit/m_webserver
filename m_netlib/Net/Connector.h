//Created by mars on 6/8/24

#ifndef MARS_NET_CONNECTOR_H
#define MARS_NET_CONNECTOR_H

#include "../Base/noncopyable.h"
#include "../Socket/InetAddress.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TimerId.h"

#include <functional>

namespace mars{
namespace net{

class Connector : noncopyable{
public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb) {m_newConnectionCallback = cb;}

    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress() const { return m_serverAddr; }


private:
    enum States {kDisconnected, kConnecting, kConnected};
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) {m_state = s;}
    void startInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* m_loop;
    InetAddress m_serverAddr;
    bool m_connect;
    States m_state;
    std::unique_ptr<Channel> m_Channel;
    NewConnectionCallback m_newConnectionCallback;
    int m_retryDelayMs;
    TimerId m_timerId;
};

typedef std::shared_ptr<Connector> ConnectorPtr;

}
}

#endif //MARS_CONNECTOR_H
