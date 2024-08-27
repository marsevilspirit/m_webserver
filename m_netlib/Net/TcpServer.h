//Created by mars on 3/8/24

#ifndef MARS_NET_TCP_SERVER_H
#define MARS_NET_TCP_SERVER_H

#include "../Base/noncopyable.h"
#include "../Socket/InetAddress.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

#include <map>

namespace mars{
namespace net{

class TcpServer : noncopyable{
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();

    void setThreadNum(int numThreads);

    void start();

    void setConnectionCallback(const ConnectionCallback& cb){ m_connectionCallback = cb;}

    void setMessageCallback(const MessageCallback& cb){ m_messageCallback = cb;}

    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ m_writeCompleteCallback = cb;}

    EventLoop* getLoop() const { return m_loop; }

private:

    void newConnection(int sockfd, const InetAddress& peerAddr);

    void removeConnection(const TcpConnectionPtr& conn);

    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* m_loop; //main loop
    const std::string m_name;
    std::unique_ptr<Acceptor> m_acceptor;
    std::shared_ptr<EventLoopThreadPool> m_threadPool; // sub loops

    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    bool m_started;
    int m_nextConnId;
    ConnectionMap m_connections;
};

} //namespace net
} //namespace mars


#endif //MARS_NET_TCP_SERVER_H
