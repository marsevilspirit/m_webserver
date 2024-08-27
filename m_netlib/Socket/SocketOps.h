#ifndef SOCKETOPS_H
#define SOCKETOPS_H

#include <arpa/inet.h>
#include <unistd.h>

namespace mars{
namespace sockets{

    int createNonblockingOrDie();

    void bindOrDie(int sockfd, const struct sockaddr_in& addr);
    void listenOrDie(int sockfd);
    int accept(int sockfd, struct sockaddr_in* addr);
    void close(int sockfd);
    int connect(int sockfd, const struct sockaddr_in& addr);

    void setNonBlockAndCloseOnExec(int sockfd);

    struct sockaddr_in getLocalAddr(int sockfd);

    struct sockaddr_in getPeerAddr(int sockfd);

    int getSocketError(int sockfd);

    void shutdownWrite(int sockfd);

    bool isSelfConnect(int sockfd);

} // namespace sockets
} // namespace mars

#endif // SOCKETOPS_H
