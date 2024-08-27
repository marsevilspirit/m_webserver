#include "EPoller.h"
#include "../Log/mars_logger.h"
#include "Channel.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <poll.h>

using namespace mars;
using namespace mars::net;

static_assert(EPOLLIN == POLLIN, "EPOLLIN and POLLIN must be equal");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI and POLLPRI must be equal");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT and POLLOUT must be equal");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP and POLLRDHUP must be equal");
static_assert(EPOLLERR == POLLERR, "EPOLLERR and POLLERR must be equal");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUP and POLLHUP must be equal");

namespace {
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

EPoller::EPoller(EventLoop* loop)
    : m_loop(loop),
      m_epollfd(::epoll_create1(EPOLL_CLOEXEC)),
      m_events(kInitEventListSize)
{
    if (m_epollfd < 0) {
        LogFatal("EPoller::EPoller");
    }
}

EPoller::~EPoller()
{
    ::close(m_epollfd);
}

base::Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(m_epollfd, &m_events.front(), static_cast<int>(m_events.size()), timeoutMs);
    base::Timestamp now(base::Timestamp::now());
    if (numEvents > 0) {
        LogDebug("EPoller::poll() %d events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == m_events.size()) {
            m_events.resize(m_events.size() * 2);
        }
    } else if (numEvents == 0) {
        LogDebug("EPoller::poll() nothing happened");
    } else {
        LogError("EPoller::poll() error: {}", strerror(errno));
    }
    return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= m_events.size());
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
        channel->set_revents(m_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if (index == kNew) {
            assert(m_channels.find(fd) == m_channels.end());
            m_channels[fd] = channel;
        } else {
            assert(m_channels.find(fd) != m_channels.end());
            assert(m_channels[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        assert(m_channels.find(fd) != m_channels.end());
        assert(m_channels[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel* channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    assert(m_channels.find(fd) != m_channels.end());
    assert(m_channels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = m_channels.erase(fd);
    assert(n == 1);
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(m_epollfd, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LogError("EPoller::update() EPOLL_CTL_DEL");
        } else {
            LogFatal("EPoller::update() EPOLL_CTL_ADD or EPOLL_CTL_MOD");
        }
    }
}
