//Created by mars on 2/8/24

#ifndef MARS_NET_TIMER_H
#define MARS_NET_TIMER_H

#include "../Base/noncopyable.h"
#include "Callbacks.h"
#include "../Base/Timestamp.h"

#include <atomic>

namespace mars{

using base::Timestamp;

namespace net{

class Timer : noncopyable{
public:
    Timer(TimerCallback cb, Timestamp when, double interval)
      :  m_cb(std::move(cb)),
         m_expiration(when),
         m_interval(interval),
         m_repeat(interval > 0.0),
         m_sequence(++s_numCreated)
    {}

    void run() const {m_cb();}

    Timestamp expiration() const { return m_expiration; }
    bool repeat() const { return m_repeat; }
    int64_t sequence() const { return m_sequence; }

    void restart(Timestamp now);

private:
    const TimerCallback m_cb;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
    const int64_t m_sequence;

    static std::atomic<int64_t> s_numCreated;
};

} // namespace net
} // namespace mars

#endif // MARS_NET_TIMER_H
