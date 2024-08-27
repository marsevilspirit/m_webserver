#include "TcpClient.h"
#include "../Log/mars_logger.h"
#include "../Socket/SocketOps.h"

using namespace mars;
using namespace mars::net;

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{
    //connector->
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr)
    : m_loop(loop),
      m_connector(new Connector(loop, serverAddr)),
      m_nextConnId(1),
      m_retry(false),
      m_connect(true)
{
    m_connector->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));

    LogInfo("TcpClient::TcpClient[{}] - connector {}", m_name, (void *)m_connector.get());
}

TcpClient::~TcpClient()
{
    LogInfo("TcpClient::~TcpClient[{}] - connector {}", m_name, (void *)m_connector.get());
    TcpConnectionPtr conn;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        conn = m_connection;
    }
    if (conn)
    {
        CloseCallback cb = std::bind(&TcpClient::removeConnection, this, std::placeholders::_1);
        m_loop->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
    }
    else
    {
        m_connector->stop();
    }
}

void TcpClient::connect()
{
    LogInfo("TcpClient::connect[{}] - connecting to {}", m_name, m_connector->serverAddress().toHostPort());
    m_connect = true;
    m_connector->start();
}

void TcpClient::disconnect()
{
    m_connect = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_connection)
        {
            m_connection->shutdown();
        }
    }
}

void TcpClient::stop()
{
    m_connect = false;
    m_connector->stop();
}

void TcpClient::newConnection(int sockfd)
{
    m_loop->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toHostPort().c_str(), static_cast<int>(m_nextConnId));
    ++m_nextConnId;
    std::string connName = m_name + buf;

    LogInfo("TcpClient::newConnection[{}] - new connection [{}] from {}", m_name, connName, peerAddr.toHostPort());

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(m_loop, connName, sockfd, localAddr, peerAddr));
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(
        std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connection = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    m_loop->assertInLoopThread();
    LogInfo("TcpClient::removeConnection[{}] - connection {}", m_name, conn->name());
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_connection == conn);
        m_connection.reset();
    }

    m_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (m_retry && m_connect)
    {
        LogInfo("TcpClient::removeConnection[{}] - Reconnecting to {}", m_name, m_connector->serverAddress().toHostPort());
        m_connector->restart();
    }
}
