#include "Acceptor.h"
#include "../Socket/InetAddress.h"
#include "../Socket/SocketOps.h"
#include "EventLoop.h"

using namespace mars;
using namespace mars::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
    : m_loop(loop),
      m_acceptSocket(sockets::createNonblockingOrDie()),
      m_acceptChannel(loop, m_acceptSocket.fd()),
      m_listenning(false)
{
    m_acceptSocket.setReuseAddr(true);
    m_acceptSocket.bindAddress(listenAddr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen(){
    m_loop->assertInLoopThread();
    m_listenning = true;
    m_acceptSocket.listen();
    m_acceptChannel.enableReading();
}

void Acceptor::handleRead(){
    m_loop->assertInLoopThread();
    InetAddress peerAddr(0);

    int connfd = m_acceptSocket.accept(&peerAddr);
    if (connfd >= 0) {
        if (m_newConnectionCallback){
            m_newConnectionCallback(connfd, peerAddr);
        } else {
            sockets::close(connfd);
        }
    }
}
