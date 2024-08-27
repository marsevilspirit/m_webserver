//Created by mars 3/8/24

#ifndef MARS_NET_EVENTLOOPTHREAD_H
#define MARS_NET_EVENTLOOPTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../Base/noncopyable.h"

namespace mars{
namespace net{

class EventLoop;

class EventLoopThread : noncopyable{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    std::thread m_thread;
    bool m_exiting;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    EventLoop* m_eventLoop;
};

} // namespace net
} // namespace mars

#endif
