#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include "../Log/mars_logger.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <assert.h>

namespace mars{
namespace net{
namespace detail{

int createTimerfd(){
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LogFatal("Failed in timerfd_create");
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when){
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100){
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now){// 读取 timerfd 的数据, 以免一直触发
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    LogTrace("TimerQueue::readTimerfd() {} at {}", howmany, now.toString().c_str());
    if(n != sizeof(howmany)){
        LogError("TimerQueue::readTimerfd() reads %lu bytes instead of 8", n);
    }
}

void resetTimerfd(int timerfd, Timestamp expiration){
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret){
        LogError("timerfd_settime()");
    }
}

} // namespace detail
} // namespace net
} // namespace mars

using namespace mars;
using namespace mars::net;
using namespace mars::net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : m_loop(loop),
    m_timerfd(createTimerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers(),
    m_callingExpiredTimers(false)
{
    m_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enableReading();//加入poller监听
}

TimerId TimerQueue::addTimer(const TimerCallback cb, Timestamp when, double interval){
    Timer* timer = new Timer(cb, when, interval);

    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));

    return TimerId(timer);
}

void TimerQueue::cancel(TimerId timerId){
    m_loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    m_loop->assertInLoopThread();
    bool earliestChanged = insert(timer);

    //如果最早的定时器发生变化, 则重置 timerfd
    if(earliestChanged){
        resetTimerfd(m_timerfd, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    m_loop->assertInLoopThread();
    assert(m_timers.size() == m_activeTimers.size());
    ActiveTimer timer(timerId.m_timer, timerId.m_seq);
    auto it = m_activeTimers.find(timer);
    if (it != m_activeTimers.end()){
        size_t n = m_timers.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); (void)n;
        delete it->first; // FIXME: no delete please
        m_activeTimers.erase(it);
    } else if (m_callingExpiredTimers){
        auto it = m_cancelingTimers.find(timer);
        if (it != m_cancelingTimers.end()){
            size_t n = m_timers.erase(Entry(it->first->expiration(), it->first));
            assert(n == 1); (void)n;
            delete it->first; // FIXME: no delete please
            m_cancelingTimers.erase(it);
        }
    }
}

void TimerQueue::handleRead(){
    Timestamp now(Timestamp::now());
    readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);

    m_callingExpiredTimers = true;
    m_cancelingTimers.clear();

    for(auto& entry : expired){
        entry.second->run();
    }

    m_callingExpiredTimers = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now){
    assert(m_timers.size() == m_activeTimers.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto end = m_timers.lower_bound(sentry);
    assert(end == m_timers.end() || now < end->first);
    std::copy(m_timers.begin(), end, std::back_inserter(expired));
    m_timers.erase(m_timers.begin(), end);

    for(auto& entry : expired){
        ActiveTimer timer(entry.second, entry.second->sequence());
        size_t n = m_activeTimers.erase(timer);
        assert(n == 1); (void)n;
    }

    assert(m_timers.size() == m_activeTimers.size());
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now){
    Timestamp nextExpire;

    for(auto& entry : expired){
        ActiveTimer timer(entry.second, entry.second->sequence());
        if(entry.second->repeat() && m_cancelingTimers.find(timer) == m_cancelingTimers.end()){
            entry.second->restart(now);
            insert(entry.second);
        }else{
            delete entry.second;
        }
    }    

    if(!m_timers.empty()){
        nextExpire = m_timers.begin()->second->expiration();
    }

    if(nextExpire.valid()){
        resetTimerfd(m_timerfd, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer){
    m_loop->assertInLoopThread();
    assert(m_timers.size() == m_activeTimers.size());

    bool earliestChanged = false;
    Timestamp when = timer->expiration();

    auto it = m_timers.begin();
    if(it == m_timers.end() || when < it->first){
        earliestChanged = true;
    }

    {
        std::pair<TimerList::iterator, bool> result = m_timers.insert(Entry(when, timer));
        assert(result.second);
        (void)result;
    }

    {
        std::pair<ActiveTimerSet::iterator, bool> result = m_activeTimers.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
        (void)result;
    }

    assert(m_timers.size() == m_activeTimers.size());
    return earliestChanged;
}

TimerQueue::~TimerQueue(){
    ::close(m_timerfd);
    for(auto& timer : m_timers){
        delete timer.second;
    }
}
