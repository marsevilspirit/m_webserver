//Created by mars on 2/8/24

#ifndef MARS_NET_CALLBACKS_H
#define MARS_NET_CALLBACKS_H

#include "../Base/Timestamp.h"
#include "Buffer.h"

#include <functional>
#include <memory>

namespace mars{

class InetAddress;

namespace net{
// All client visible callbacks go here.

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void()> Functor;
typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer* buf, base::Timestamp)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

}
}

#endif // MARS_NET_CALLBACKS_H
