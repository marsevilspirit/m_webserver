//Created by mars on 27/8/24

#ifndef MARS_HTTP_REQUEST_H
#define MARS_HTTP_REQUEST_H

#include "m_netlib/Base/copyable.h"
#include "m_netlib/Base/Timestamp.h"

#include <map>
#include <string>
#include <assert.h>

namespace mars{
namespace net{

class HttpRequest : public base::copyable{
public:
    enum Method{
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };

    enum Version{
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
        : m_method(kInvalid),
          m_version(kUnknown)
    {}

    void setVersion(Version v){ m_version = v; }

    Version getVersion() const { return m_version; }

    bool setMethod(const char* start, const char* end)
    {
        assert(m_method == kInvalid);
        std::string m(start, end);
        if(m == "GET"){
            m_method = kGet;
        }else if(m == "POST"){
            m_method = kPost;
        }else if(m == "HEAD"){
            m_method = kHead;
        }else if(m == "PUT"){
            m_method = kPut;
        }else if(m == "DELETE"){
            m_method = kDelete;
        }else{
            m_method = kInvalid;
        }
        return m_method != kInvalid;
    }

    Method method() const { return m_method; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(m_method){
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        m_path.assign(start, end);
    }

    const std::string& path() const { return m_path; }

    void setQuery(const char* start, const char* end)
    {
        m_query.assign(start, end);
    }

    const std::string& query() const { return m_query; }

    void setReceiveTime(base::Timestamp t){ m_receiveTime = t; }

    base::Timestamp receiveTime() const { return m_receiveTime; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        while(colon < end && isspace(*colon)){
            ++colon;
        }
        std::string value(colon, end);
        while(!value.empty() && isspace(value[value.size() - 1])){
            value.resize(value.size() - 1);
        }
        m_headers[field] = value;
    }

    std::string getHeader(const std::string& field) const
    {
        std::string result;
        auto it = m_headers.find(field);
        if(it != m_headers.end()){
            result = it->second;
        }
        return result;
    }

    const std::map<std::string, std::string>& headers() const { return m_headers; }

    void swap(HttpRequest& that)
    {
        std::swap(m_method, that.m_method);
        m_path.swap(that.m_path);
        m_query.swap(that.m_query);
        m_receiveTime.swap(that.m_receiveTime);
        m_headers.swap(that.m_headers);
    }

private:
    Method m_method;
    Version m_version;
    std::string m_path;
    std::string m_query;
    base::Timestamp m_receiveTime;
    std::map<std::string, std::string> m_headers;
};

}
}

#endif //MARS_HTTP_REQUEST_H
