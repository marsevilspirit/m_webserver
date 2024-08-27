//Created by mars on 2/8/24

#ifndef MARS_NET_TIMERQUEUE_H
#define MARS_NET_TIMERQUEUE_H

#include "../Base/noncopyable.h"
#include "Callbacks.h"
#include "../Base/Timestamp.h"
#include "Channel.h"

#include <memory>
#include <set>

namespace mars{
namespace net{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : noncopyable{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback cb, base::Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<base::Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    void handleRead();
    std::vector<Entry> getExpired(base::Timestamp now);
    void reset(const std::vector<Entry>& expired, base::Timestamp now);

    bool insert(Timer* timer);

    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timers;

  // for cancel()
  bool m_callingExpiredTimers; /* atomic */
  ActiveTimerSet m_activeTimers;
  ActiveTimerSet m_cancelingTimers;
};

}
}

#endif // MARS_NET_TIMERQUEUE_H
