// Created by mars on 2/8/24.

#ifndef MARS_NET_POLLER_H
#define MARS_NET_POLLER_H

#include "../Base/noncopyable.h"
#include "EventLoop.h"
#include "../Base/Timestamp.h"
#include <vector>
#include <map>

struct pollfd;

namespace mars {
namespace net {

class Channel;

class Poller : noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller();

    base::Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);

    void removeChannel(Channel* channel);

    void assertInLoopThread() { m_ownerLoop->assertInLoopThread(); }

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* m_ownerLoop;
    PollFdList m_pollfds;
    ChannelMap m_channels;
};

} // namespace net
} // namespace mars

#endif // MARS_NET_POLLER_H
