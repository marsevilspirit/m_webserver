#include "TcpConnection.h"
#include "Channel.h"
#include "../Log/mars_logger.h"
#include "../Socket/SocketOps.h"

#include <assert.h>

using namespace mars;
using namespace mars::net;

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
    : m_loop(loop),
      m_name(name),
      m_state(kConnecting),
      m_socket(new Socket(sockfd)),
      m_channel(new Channel(loop, sockfd)),
      m_localAddr(localAddr),
      m_peerAddr(peerAddr)
{
    LogInfo("TcpConnection::ctor[{}] at {} fd = {}", m_name, m_localAddr.toHostPort(), sockfd);
    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection(){
    LogInfo("TcpConnection::dtor[{}] at {} fd = {}", m_name, m_localAddr.toHostPort(), m_channel->fd());
}

void TcpConnection::connectDestroyed(){
    m_loop->assertInLoopThread();

    LogTrace("m_state = {}", static_cast<int>(m_state));

    assert(m_state == kConnected || m_state == kDisconnecting);
    setState(kDisconnected);
    m_channel->disableAll();
    m_connectionCallback(shared_from_this());

    m_loop->removeChannel(m_channel.get());
}

void TcpConnection::connectEstablished(){
    m_loop->assertInLoopThread();
    assert(m_state == kConnecting);
    setState(kConnected);
    m_channel->enableReading();
    m_connectionCallback(shared_from_this());
}

void TcpConnection::handleRead(base::Timestamp receiveTime){
    int savedErrno = 0;
    ssize_t n = m_inputBuffer.readFd(m_channel->fd(), &savedErrno);

    if (n > 0){
        m_messageCallback(shared_from_this(), &m_inputBuffer, receiveTime);
    }
    else if (n == 0){
        handleClose();
    } else {
        errno = savedErrno;
        LogError("TcpConnection::handleRead [{}] - SO_ERROR = {} {}", m_name, savedErrno, strerror(savedErrno));
        handleError();
    }
}

void TcpConnection::handleClose(){
    m_loop->assertInLoopThread();
    assert(m_state == kConnected || m_state == kDisconnecting);
    LogInfo("enter TcpConnection::handleClose [{}] - SO_ERROR = 0", m_name);
    m_channel->disableAll();
    LogTrace("disableAll() done");
    m_closeCallback(shared_from_this());
    LogTrace("closeCallback() done");
}

void TcpConnection::handleError(){
    int err = sockets::getSocketError(m_channel->fd());
    LogError("TcpConnection::handleError [{}] - SO_ERROR = {} {}", m_name, err, strerror(err));
}

void TcpConnection::shutdown(){
    if (m_state == kConnected){
        setState(kDisconnecting);
        m_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::setTcpNoDelay(bool on){
    m_socket->setTcpNoDelay(on);
}

void TcpConnection::shutdownInLoop(){
    m_loop->assertInLoopThread();
    if (!m_channel->isWriting()){
        m_socket->shutdownWrite();
    }
}

// FIXME efficiency!!!
void TcpConnection::send(Buffer* buf)
{
  if (m_state == kConnected)
  {
    if (m_loop->isInLoopThread())
    {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    }
    else
    {
      void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
      m_loop->runInLoop(
          std::bind(fp,
                    this,     // FIXME
                    buf->retrieveAsString()));
                    //std::forward<string>(message)));
    }
  }
}

void TcpConnection::send(const void* data, int len)
{
  send(StringPiece(static_cast<const char*>(data), len));
}

void TcpConnection::send(const StringPiece& message)
{
  if (m_state == kConnected)
  {
    if (m_loop->isInLoopThread())
    {
      sendInLoop(message);
    }
    else
    {
      void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
      m_loop->runInLoop(
          std::bind(fp,
                    this,     // FIXME
                    message.as_string()));
                    //std::forward<string>(message)));
    }
  }
}

void TcpConnection::sendInLoop(const StringPiece& message){
    m_loop->assertInLoopThread();
    ssize_t nwrote = 0;
    if (!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0) {
        nwrote = ::write(m_channel->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (static_cast<size_t>(nwrote) < message.size()) {
                LogTrace("I am going to wrtie more data");
            } else if (m_writeCompleteCallback) {
                m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LogError("TcpConnection::sendInLoop");
            }
        }
    }

    assert(nwrote >= 0);
    if (static_cast<size_t>(nwrote) < message.size()) {
        m_outputBuffer.append(message.data() + nwrote, message.size() - nwrote);
        if (!m_channel->isWriting()) {
            m_channel->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
  m_loop->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (m_state == kDisconnected)
  {
    return;
  }
  // if no thing in output queue, try writing directly
  if (!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0)
  {
    nwrote = ::write(m_channel->fd(), data, len);
    if (nwrote >= 0)
    {
      remaining = len - nwrote;
      if (remaining == 0 && m_writeCompleteCallback)
      {
        m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
      }
    }
    else // nwrote < 0
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if (!faultError && remaining > 0)
  {
    size_t oldLen = m_outputBuffer.readableBytes();
    if (oldLen + remaining >= m_highWaterMark
        && oldLen < m_highWaterMark
        && m_highWaterMarkCallback)
    {
      m_loop->queueInLoop(std::bind(m_highWaterMarkCallback, shared_from_this(), oldLen + remaining));
    }
    m_outputBuffer.append(static_cast<const char*>(data)+nwrote, remaining);
    if (!m_channel->isWriting())
    {
      m_channel->enableWriting();
    }
  }
}

void TcpConnection::handleWrite(){
    m_loop->assertInLoopThread();
    if (m_channel->isWriting()) {
        ssize_t n = ::write(m_channel->fd(), m_outputBuffer.peek(), m_outputBuffer.readableBytes());
        if (n > 0) {
            m_outputBuffer.retrieve(n);
            if(m_outputBuffer.readableBytes() == 0) {
                m_channel->disableWriting();
                if (m_writeCompleteCallback) {
                    m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
                }
                if (m_state == kDisconnecting) {
                    shutdownInLoop();
                }
            } else {
                LogTrace("I am going to write more data");
            }
        } else {
            LogError("TcpConnection::handleWrite");
        }
    } else {
        LogTrace("Connection fd = {} is down, no more writing", m_channel->fd());
    }
}
