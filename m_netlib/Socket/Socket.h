#ifndef SOCKET_H
#define SOCKET_H

#include "InetAddress.h"

namespace mars{

class Socket {
public:
    explicit Socket(int sockfd);
    ~Socket();

    int fd() const { return m_sockfd; }

    void bindAddress(const InetAddress& localaddr);
    void listen() const;
    int accept(InetAddress* peeraddr);

    void setReuseAddr(bool on) const;

    void setTcpNoDelay(bool on);

    void shutdownWrite() const;

private:
    const int m_sockfd;
};

} // namespace mars

#endif // SOCKET_H
