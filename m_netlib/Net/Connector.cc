#include "Connector.h"
#include "../Log/mars_logger.h"
#include "../Socket/SocketOps.h"

using namespace mars;
using namespace mars::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : m_loop(loop),
      m_serverAddr(serverAddr),
      m_connect(false),
      m_state(kDisconnected),
      m_retryDelayMs(kInitRetryDelayMs)
{
    LogDebug("Connector::Connector[addr={}]", m_serverAddr.toHostPort());
}

Connector::~Connector()
{
    LogDebug("Connector::~Connector[addr={}]", m_serverAddr.toHostPort());
}

void Connector::start(){
    m_connect = true;
    m_loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop(){
    m_loop->assertInLoopThread();
    assert(m_state == kDisconnected);
    if(m_connect){
        connect();
    } else {
        LogDebug("do not connect");
    }
}

void Connector::connect(){
    int sockfd = sockets::createNonblockingOrDie();
    int ret = sockets::connect(sockfd, m_serverAddr.getSockAddrInet());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno){
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;
        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LogError("connect error in Connector::startInLoop {}", std::to_string(savedErrno));
            sockets::close(sockfd);
            break;
        default:
            LogError("Unexpected error in Connector::startInLoop {}", std::to_string(savedErrno));
            sockets::close(sockfd);
            break;
    }
}

void Connector::restart(){
    m_loop->assertInLoopThread();
    setState(kDisconnected);
    m_retryDelayMs = kInitRetryDelayMs;
    m_connect = true;
    startInLoop();
}

void Connector::stop(){
    m_connect = false;
    m_loop->cancel(m_timerId);
}

void Connector::connecting(int sockfd){
    setState(kConnecting);
    assert(!m_Channel);
    m_Channel.reset(new Channel(m_loop, sockfd));
    m_Channel->setWriteCallback(std::bind(&Connector::handleWrite, this));
    m_Channel->setErrorCallback(std::bind(&Connector::handleError, this));
    m_Channel->enableWriting();
}

int Connector::removeAndResetChannel(){
    m_Channel->disableAll();
    m_loop->removeChannel(m_Channel.get());
    int sockfd = m_Channel->fd();
    m_loop->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel(){
    m_Channel.reset();
}

void Connector::handleWrite(){
    LogTrace("Connector::handleWrite state={}", std::to_string(m_state));
    if(m_state == kConnecting){
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err){
            LogError("Connector::handleWrite - SO_ERROR = {}", std::to_string(err));
            retry(sockfd);
        } else if(sockets::isSelfConnect(sockfd)){
            LogError("Connector::handleWrite - Self connect");
            retry(sockfd);
        } else {
            setState(kConnected);
            if(m_connect){
                m_newConnectionCallback(sockfd);
            } else {
                sockets::close(sockfd);
            }
        }
    } else {
        assert(m_state == kDisconnected);
    }
}

void Connector::handleError(){
    LogError("Connector::handleError state={}", std::to_string(m_state));
    assert(m_state == kConnecting);

    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    LogError("SO_ERROR = {}", std::to_string(err));
    retry(sockfd);
}

void Connector::retry(int sockfd){
    sockets::close(sockfd);
    setState(kDisconnected);
    if(m_connect){
        LogInfo("Connector::retry - Retry connecting to {}", m_serverAddr.toHostPort());
        m_timerId = m_loop->runAfter(m_retryDelayMs/1000.0, std::bind(&Connector::startInLoop, this));
        m_retryDelayMs = std::min(m_retryDelayMs*2, kMaxRetryDelayMs);
    } else {
        LogDebug("do not connect");
    }
}
