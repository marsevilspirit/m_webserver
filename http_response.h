//Created by mars on 27/8/24

#ifndef MARS_HTTP_RESPONSE_H
#define MARS_HTTP_RESPONSE_H

#include "m_netlib/Base/copyable.h"
#include "m_netlib/Net/Buffer.h"

#include <map>

namespace mars {
namespace net {

class HttpResponse : public mars::base::copyable {
public:
    enum HttpStatusCode {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404
    };

    explicit HttpResponse(bool close)
        : m_statusCode(kUnknown),
          m_closeConnection(close)
    {}

    void setStatusCode(HttpStatusCode code)
    { m_statusCode = code; }

    void setStatusMessage(const std::string& message)
    { m_statusMessage = message; }

    void setCloseConnection(bool on)
    { m_closeConnection = on; }

    bool closeConnection() const
    { return m_closeConnection; }

    void setContentType(const std::string& contentType)
    { addHeader("Content-Type", contentType); }

    void addHeader(const std::string& key, const std::string& value)
    { m_headers[key] = value; }

    void setBody(const std::string& body)
    { m_body = body; }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<std::string, std::string> m_headers;
    HttpStatusCode m_statusCode;
    std::string m_statusMessage;
    bool m_closeConnection;
    std::string m_body;
};

} //namespace net
} //namespace mars

#endif //MARS_HTTP_RESPONSE_H
