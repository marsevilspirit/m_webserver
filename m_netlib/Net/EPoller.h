#ifndef MARS_NET_EPOLLER_H_
#define MARS_NET_EPOLLER_H_

#include <map>
#include <vector>

#include "../Base/Timestamp.h"
#include "EventLoop.h"
#include "../Base/noncopyable.h"

struct epoll_event;

namespace mars {
namespace net {

class Channel;

class EPoller : noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    EPoller(EventLoop* loop);
    ~EPoller();

    base::Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread() const {m_loop->assertInLoopThread();}

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* m_loop;
    int m_epollfd;
    EventList m_events;
    ChannelMap m_channels;
};

} // namespace net
} // namespace mars

#endif // MARS_NET_EPOLLER_H_
