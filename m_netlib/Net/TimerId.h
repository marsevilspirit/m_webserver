//Created by mars on 2/8/24

#ifndef MARS_NET_TIMERID_H
#define MARS_NET_TIMERID_H

#include "../Base/copyable.h"
#include <cstdint>

namespace mars{
namespace net{

class Timer;

class TimerId : public base::copyable{
public:
    TimerId(Timer* timer = nullptr, int64_t seq = 0)
        : m_timer(timer),
          m_seq(seq)
    {
    }

    friend class TimerQueue;

private:
    Timer* m_timer;
    int64_t m_seq;
};

}
}

#endif // MARS_NET_TIMERID_H
