//Created by mars on 6/8/24

#ifndef __TcpClient_H__
#define __TcpClient_H__

#include <map>
#include <string>
#include <mutex>

#include "TcpConnection.h"
#include "Callbacks.h"
#include "Connector.h"

namespace mars{
namespace net{

class TcpClient{
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_connection;
    }

    bool retry() const;
    void enableRetry() {m_retry = true;}

    void setConnectionCallback(const ConnectionCallback& cb){m_connectionCallback = cb;}
    void setMessageCallback(const MessageCallback& cb){m_messageCallback = cb;}
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){m_writeCompleteCallback = cb;}

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* m_loop;
    ConnectorPtr m_connector;
    const std::string m_name;
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    CloseCallback m_closeCallback;
    bool m_retry;
    bool m_connect;

    size_t m_nextConnId;
    std::mutex m_mutex;
    TcpConnectionPtr m_connection; 
};

} //namespace net
} //namespace mars

#endif //__TcpClient_H__
