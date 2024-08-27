#include "http_context.h"

using namespace mars;
using namespace mars::net;
using namespace mars::base;

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    HttpRequest::Method method = HttpRequest::kInvalid;
    if(space != end){
        if(m_request.setMethod(start, space)){
            start = space + 1;
            space = std::find(start, end, ' ');
            if(space != end){
                const char* question = std::find(start, space, '?');
                if(question != space){
                    m_request.setPath(start, question);
                    m_request.setQuery(question, space);
                }else{
                    m_request.setPath(start, space);
                }
                start = space + 1;
                succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
                if(succeed){
                    if(*(end - 1) == '1'){
                        m_request.setVersion(HttpRequest::kHttp11);
                    }else if(*(end - 1) == '0'){
                        m_request.setVersion(HttpRequest::kHttp10);
                    }else{
                        succeed = false;
                    }
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore){
        if(m_state == kExpectRequestLine){
            const char* crlf = buf->findCRLF();
            if(crlf){
                ok = processRequestLine(buf->peek(), crlf);
                if(ok){
                    m_request.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    m_state = kExpectHeaders;
                }else{
                    hasMore = false;
                }
            }else{
                hasMore = false;
            }
        }else if(m_state == kExpectHeaders){
            const char* crlf = buf->findCRLF();
            if(crlf){
                const char* colon = std::find(buf->peek(), crlf, ':');
                if(colon != crlf){
                    m_request.addHeader(buf->peek(), colon, crlf);
                }else{
                    m_state = kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            }else{
                hasMore = false;
            }
        }else if(m_state == kExpectBody){
            //TODO
        }
    }
    return ok;
}
