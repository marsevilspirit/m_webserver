#include "EventLoopThreadPool.h"

using namespace mars;
using namespace mars::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : m_baseLoop(baseLoop),
      m_started(false),
      m_numThreads(0),
      m_next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(){
    m_baseLoop->assertInLoopThread();
    m_started = true;
    for(int i = 0; i < m_numThreads; i++){
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        m_threads.push_back(t);
        m_loops.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    m_baseLoop->assertInLoopThread();
    EventLoop* loop = m_baseLoop;
    if(!m_loops.empty()){
        loop = m_loops[m_next];
        m_next = (m_next + 1) % m_loops.size();
    }
    return loop;
}
