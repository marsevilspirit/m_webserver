#include "Socket.h"
#include "SocketOps.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>

using namespace mars;

Socket::Socket(int sockfd) : m_sockfd(sockfd) 
{
}

Socket::~Socket() 
{
    sockets::close(m_sockfd);
}

void Socket::bindAddress(const InetAddress& localaddr) 
{
    sockets::bindOrDie(m_sockfd, localaddr.getSockAddrInet());
}

void Socket::listen() const 
{
    sockets::listenOrDie(m_sockfd);
}

int Socket::accept(InetAddress* peeraddr) 
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int connfd = sockets::accept(m_sockfd, &addr);
    if (connfd >= 0) 
    {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void Socket::setReuseAddr(bool on) const 
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::shutdownWrite() const
{
    sockets::shutdownWrite(m_sockfd);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}
