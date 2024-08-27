#include "EventLoop.h"
#include "../Log/mars_logger.h"
#include "TimerQueue.h"
#include "EPoller.h"
#include "Channel.h"
#include <assert.h>
#include <cstdlib>
#include <sys/eventfd.h>
#include <signal.h>

using namespace mars::net;

__thread EventLoop* t_loopInThisThread = nullptr;
const int kPollTimeMs = 10000;

static int createEventfd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LogFatal("Failed in eventfd");
        abort();
    }
    return evtfd;
}

class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj;

EventLoop::EventLoop() 
    : m_looping(false), 
    m_quit(false),
    m_callingPendingFunctors(false),
    m_threadId(gettid()), 
    m_poller(new EPoller(this)),
    m_timerQueue(new TimerQueue(this)),
    m_wakeupFd(createEventfd()),
    m_wakeupChannel(new Channel(this, m_wakeupFd))
{
    LogTrace("EventLoop created {} in thread {}", (void*)this, m_threadId);
    if (t_loopInThisThread) {
        LogFatal("Another EventLoop {} exists in this thread {}", (void*)t_loopInThisThread, m_threadId);
        exit(1);
    } else {
        t_loopInThisThread = this;
    }

    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
    assert(!m_looping);
    ::close(m_wakeupFd);
    t_loopInThisThread = nullptr;
}

// 断言是否在loop线程
EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

// 断言是否在loop线程
void EventLoop::abortNotInLoopThread() {
    LogFatal("EventLoop::abortNotInLoopThread - EventLoop was created in threadId = {}, current thread id = {}",
            m_threadId, gettid());
    exit(1);
}

// 开始loop
void EventLoop::loop() {
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;
    m_quit = false;

    while (!m_quit) {
        m_activeChannels.clear();
        m_pollReturnTime = m_poller->poll(kPollTimeMs, &m_activeChannels);
        for(auto& ch : m_activeChannels) {
            ch->handleEvent(m_pollReturnTime);
        }

        doPendingFunctors();
    }

    LogTrace("EventLoop {} stop looping", (void*)this);
    m_looping = false;
}

// 退出loop
void EventLoop::quit() {
    m_quit = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

// 更新channel
void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->removeChannel(channel);
}

// 在指定时间执行回调
TimerId EventLoop::runAt(const base::Timestamp& time, const TimerCallback& cb){
    return m_timerQueue->addTimer(cb, time, 0.0);
}

// 在指定时间后执行回调
TimerId EventLoop::runAfter(double delay, const TimerCallback& cb){
    base::Timestamp time(addTime(base::Timestamp::now(), delay));
    return runAt(time, cb);
}

// 每隔interval时间执行回调
TimerId EventLoop::runEvery(double interval, const TimerCallback& cb){
    base::Timestamp time(addTime(base::Timestamp::now(), interval));
    return m_timerQueue->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId){
    return m_timerQueue->cancel(timerId);
}

// 将不在loop线程的任务放回loop线程执行
void EventLoop::runInLoop(const Functor& cb) {
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

// 将任务放回loop线程执行
void EventLoop::queueInLoop(const Functor& cb) {
    LogTrace("queueInLoop");

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pendingFunctors.push_back(cb);
    }

    LogTrace("isInLoopThread: {}, m_callingPendingFunctors: {}", isInLoopThread(), m_callingPendingFunctors);

    if(!isInLoopThread() || m_callingPendingFunctors) {
        wakeup();
    }
}

// 执行回调任务
void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    m_callingPendingFunctors = true;

    LogTrace("doing doPendingFunctors");

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for(auto& functor : functors){
        LogTrace("doing pending functor");
        functor();
    }

    m_callingPendingFunctors = false;
}

// 唤醒loop线程
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeupFd, &one, sizeof(one));
    LogTrace("write {} bytes to wakeupFd", n);
    if (n != sizeof(one)){
        LogError("EventLoop::wakeup() writes {} bytes instead of 8", n);
    }
}

// 读取wakeupFd, 防止一直触发
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(m_wakeupFd, &one, sizeof(one));
    LogTrace("read {} bytes from wakeupFd", n);
    if (n != sizeof(one)){
        LogError("EventLoop::handleRead() reads {} bytes instead of 8", n);
    }
}
