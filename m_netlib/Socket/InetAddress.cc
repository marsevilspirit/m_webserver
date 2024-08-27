#include "InetAddress.h"
#include <arpa/inet.h> // inet_pton

using namespace mars;

InetAddress::InetAddress(uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr);
    m_addr.sin_port = htons(port);
}

std::string InetAddress::toHostPort() const {
    char buf[32];
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &m_addr.sin_addr, host, sizeof(host));
    snprintf(buf, sizeof(buf), "%s:%u", host, ntohs(m_addr.sin_port));
    return buf;
}
