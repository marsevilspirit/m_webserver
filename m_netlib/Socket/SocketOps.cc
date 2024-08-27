#include "SocketOps.h"
#include "../Log/mars_logger.h"
#include <fcntl.h>
#include <sys/socket.h>

using namespace mars;

int sockets::createNonblockingOrDie()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LogError("create socket error");
    }
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
    int ret = ::bind(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
    if (ret < 0)
    {
        LogError("bind error");
    }
}

void sockets::listenOrDie(int sockfd)
{
    if (::listen(sockfd, SOMAXCONN) < 0)
    {
        LogError("listen error");
    }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) //muduo此处用了accept4
{
    socklen_t addrlen = sizeof(*addr);
    int connfd = ::accept(sockfd, reinterpret_cast<struct sockaddr*>(addr), &addrlen);

    setNonBlockAndCloseOnExec(connfd);

    if (connfd < 0)
    {
        LogError("accept error");
    }
    return connfd;

}

void sockets::close(int sockfd) {
    int ret = ::close(sockfd);
    if (ret < 0) {
        LogError("Close error!");
    }
}

int sockets::connect(int sockfd, const struct sockaddr_in& addr) {
    return ::connect(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
}

void sockets::setNonBlockAndCloseOnExec(int sockfd) {
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);

    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
}

struct sockaddr_in sockets::getLocalAddr(int sockfd) {
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    socklen_t addrlen = sizeof(localAddr);
    if (::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localAddr), &addrlen) < 0) {
        LogError("getsockname error");
    }
    return localAddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd) {
    struct sockaddr_in peerAddr;
    memset(&peerAddr, 0, sizeof(peerAddr));
    socklen_t addrlen = sizeof(peerAddr);
    if (::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&peerAddr), &addrlen) < 0) {
        LogError("getpeername error");
    }
    return peerAddr;
}

int sockets::getSocketError(int sockfd){
    int optval;
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

void sockets::shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LogError("shutdownWrite error");
    }
}

bool sockets::isSelfConnect(int sockfd) {
    struct sockaddr_in localAddr = getLocalAddr(sockfd);
    struct sockaddr_in peerAddr = getPeerAddr(sockfd);
    return localAddr.sin_port == peerAddr.sin_port && localAddr.sin_addr.s_addr == peerAddr.sin_addr.s_addr;
}
