//Created by mars on 3/8/24.

#ifndef MARS_NET_ACCEPTOR_H
#define MARS_NET_ACCEPTOR_H

#include "../Base/noncopyable.h"
#include "Callbacks.h"
#include "Channel.h"
#include "../Socket/Socket.h"

namespace mars {
namespace net {

class EventLoop;

class Acceptor : noncopyable {
public:
    Acceptor(EventLoop* loop, const InetAddress& listenAddr);

    void setNewConnectionCallback(const NewConnectionCallback& cb) {m_newConnectionCallback = cb;}

    bool listenning() const {return m_listenning;}

    void listen();

private:
    void handleRead();

    EventLoop* m_loop;
    Socket m_acceptSocket;
    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback; 
    bool m_listenning;
};

} // namespace net
} // namespace mars

#endif
