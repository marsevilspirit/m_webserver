#include "Channel.h"
#include "EventLoop.h"
#include "../Log/mars_logger.h"
#include <poll.h>
#include <assert.h>

using namespace mars::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : m_loop(loop),
      m_fd(fd),
      m_events(0),
      m_revents(0),
      m_index(-1)
{
}

Channel::~Channel(){
    assert(!m_eventHandling); //在处理时间时，Channel 对象不能被析构
}

void Channel::update(){
    m_loop->updateChannel(this);// EventLoop.h 中的 updateChannel
}

void Channel::handleEvent(base::Timestamp receiveTime){
    m_eventHandling = true;

    if(m_revents & POLLNVAL){
        LogWarn("Channel::handleEvent() POLLNVAL");// 文件描述符不是一个打开的文件
    }

    if((m_revents & POLLHUP) && !(m_revents & POLLIN)){
        LogWarn("Channel::handleEvent() POLLHUP");// 对端关闭连接
        if(m_closeCallback) m_closeCallback();
    }

    if(m_revents & (POLLERR | POLLNVAL)){
        LogError("Channel::handleEvent() POLLERR");// 出错
        if(m_errorCallback) m_errorCallback();
    }

    if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)){
        LogInfo("Channel::handleEvent() POLLIN | POLLPRI | POLLRDHUP");// 有数据可读
        if(m_readCallback) m_readCallback(receiveTime);
    }

    if(m_revents & POLLOUT){
        LogWarn("Channel::handleEvent() POLLOUT");// 可写
        if(m_writeCallback) m_writeCallback();
    }

    m_eventHandling = false;
}
