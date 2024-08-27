// Created by mars on 2/8/24.

#ifndef MARS_NET_CHANNEL_H
#define MARS_NET_CHANNEL_H

#include "../Base/noncopyable.h"
#include "../Base/Timestamp.h"
#include <functional>

namespace mars {
namespace net {

class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(base::Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(base::Timestamp receiveTime);
    void setReadCallback(const ReadEventCallback& cb) { m_readCallback = cb; }
    void setWriteCallback(const EventCallback& cb) { m_writeCallback = cb; }
    void setErrorCallback(const EventCallback& cb) { m_errorCallback = cb; }
    void setCloseCallback(const EventCallback& cb) { m_closeCallback = cb; }

    int fd() const { return m_fd; }
    int events() const { return m_events; }
    void set_revents(int revents) { m_revents = revents; }
    bool isNoneEvent() const { return m_events == kNoneEvent; }

    void enableReading() { m_events |= kReadEvent; update(); }
    void enableWriting() { m_events |= kWriteEvent; update(); }
    void disableWriting() { m_events &= ~kWriteEvent; update(); }
    void disableAll() { m_events = kNoneEvent; update(); }
    bool isWriting() const { return m_events & kWriteEvent; }

    // for Poller
    int index() { return m_index; }
    void set_index(int idx) { m_index = idx; }

    EventLoop* ownerLoop() { return m_loop; }

private:
    EventLoop* m_loop;
    int m_fd;
    int m_events;
    int m_revents;
    int m_index;
    bool m_eventHandling;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    ReadEventCallback m_readCallback;
    EventCallback m_writeCallback;
    EventCallback m_errorCallback;
    EventCallback m_closeCallback;

    void update();
};

} // namespace net
} // namespace mars

#endif // MARS_NET_CHANNEL_H
