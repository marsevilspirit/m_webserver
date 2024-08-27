//Created by mars on 27/8/24

#ifndef MARS_HTTP_CONTEXT_H
#define MARS_HTTP_CONTEXT_H

#include "m_netlib/Base/copyable.h"
#include "http_request.h"
#include "m_netlib/Base/Timestamp.h"
#include "m_netlib/Net/Buffer.h"

namespace mars{
namespace net{

class HttpContext : public mars::base::copyable{
public:
    enum HttpRequestParseState{
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };

    HttpContext()
        : m_state(kExpectRequestLine)
    {}

    bool parseRequest(Buffer* buf, base::Timestamp receiveTime);

    bool gotAll() const { return m_state == kGotAll; }

    void reset()
    {
        m_state = kExpectRequestLine;
    }

    const HttpRequest& request() const
    { return m_request; }

    HttpRequest& request()
    { return m_request; }


private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState m_state;
    HttpRequest m_request;
};

} //namespace net
} //namespace mars

#endif //MARS_HTTP_CONTEXT_H
