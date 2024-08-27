// Created by mars on 6/8/24

#ifndef __EVENTLOOPTHREADPOOL_H__
#define __EVENTLOOPTHREADPOOL_H__

#include "../Base/noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

namespace mars {
namespace net {

class EventLoopThreadPool : noncopyable {
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads) { m_numThreads = numThreads; }
    void start();
    EventLoop* getNextLoop();

private:
    EventLoop* m_baseLoop;
    bool m_started;
    int m_numThreads;
    int m_next;
    std::vector<std::shared_ptr<EventLoopThread>> m_threads;
    std::vector<EventLoop*> m_loops;
};

}
}


#endif //__EVENTLOOPTHREADPOOL_H__
