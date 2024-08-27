#include "EventLoopThread.h"
#include "EventLoop.h"

using namespace mars;
using namespace mars::net;

EventLoopThread::EventLoopThread()
    : m_thread(std::bind(&EventLoopThread::threadFunc, this)),
      m_mutex(),
      m_cond(),
      m_eventLoop(nullptr),
      m_exiting(false)
{
}

EventLoopThread::~EventLoopThread()
{
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_eventLoop == nullptr)
        {
            m_cond.wait(lock);
        }
    }
    return m_eventLoop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_eventLoop = &loop;
        m_cond.notify_one();
    }
    m_eventLoop->loop();
}
