#include "http_server.h"
#include "http_context.h"
#include "m_netlib/Log/mars_logger.h"
#include "http_response.h"

using namespace mars::net;

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr)
    : m_server(loop, listenAddr),
    m_httpCallback(defaultHttpCallback)
{
    m_server.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_server.setThreadNum(0);
}

void HttpServer::start()
{
    LogWarn("HttpServer start listenning");
    m_server.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        LogWarn("HttpServer::onConnection() new connection [{}] from {}", conn->name(), conn->peerAddress().toHostPort());
        conn->setContext(HttpContext());
    }
    else
    {
        LogWarn("HttpServer::onConnection() connection [{}] is down", conn->name());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, base::Timestamp receiveTime)
{
    HttpContext* context = std::any_cast<HttpContext>(conn->getMutableContext());
    if(!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if(context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();
    }
}


void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    //取出头部
    const std::string& connection = req.getHeader("Connection");
    // 如果connection为close或者1.0版本不支持keep-alive，标志着我们处理完请求要关闭连接
    bool close = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    //使用close构造一个HttpResponse对象，该对象可以通过方法.closeConnection()判断是否关闭连接
    HttpResponse response(close);
    //执行用户注册的回调函数
    m_httpCallback(req, &response); 
    Buffer buf;   //用户处理后的信息，追加到缓冲区
    response.appendToBuffer(&buf);
    LogError("onRequest response");
    conn->send(&buf);  //发送数据
    if (response.closeConnection())  //如果关闭
    {
        conn->shutdown();  //关了它
    }
}
