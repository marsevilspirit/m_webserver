#include "TcpServer.h"
#include "../Log/mars_logger.h"
#include "../Socket/SocketOps.h"
#include "EventLoop.h"

#include <assert.h>

using namespace mars;
using namespace mars::net;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : m_loop(loop),
      m_name(listenAddr.toHostPort()),
      m_acceptor(new Acceptor(loop, listenAddr)),
      m_threadPool(new EventLoopThreadPool(loop)),
      m_connectionCallback(),
      m_messageCallback(),
      m_started(false),
      m_nextConnId(1)
{
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){
}

void TcpServer::start(){
    if (!m_started){
        m_started = true;
        m_threadPool->start();
    }

    if (!m_acceptor->listenning()){
        m_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr){
    m_loop->assertInLoopThread();
    
    std::string connName = m_name + "#" + std::to_string(m_nextConnId++);

    LogInfo("TcpServer::newConnection [{}] - new connection [{}] from {}",
            m_name, connName, peerAddr.toHostPort());

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    EventLoop* ioLoop = m_threadPool->getNextLoop();

    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    m_connections[connName] = conn;
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::setThreadNum(int numThreads){
    LogTrace("assert(0 <= numThreads) = {}", 0 <= numThreads);
    assert(0 <= numThreads);
    m_threadPool->setThreadNum(numThreads);
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    m_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn){
    m_loop->assertInLoopThread();
    LogInfo("TcpServer::removeConnectionInLoop [{}] - connection {}", m_name, conn->name());
    size_t n = m_connections.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
