#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <string>
#include <netinet/in.h>
#include <string.h>

namespace mars {

class InetAddress {
public:
    explicit InetAddress(uint16_t port);

    InetAddress(const struct sockaddr_in& addr) : m_addr(addr) {}

    InetAddress(const std::string& ip, uint16_t port);

    const struct sockaddr_in& getSockAddrInet() const { return m_addr; }
    void setSockAddrInet(const struct sockaddr_in& addr) { m_addr = addr; } 

    std::string toHostPort() const;

private:
    struct sockaddr_in m_addr;
};

} // namespace mars

#endif // INETADDRESS_H
