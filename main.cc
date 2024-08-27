#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include "m_netlib/Net/EventLoop.h"

#include <iostream>

using namespace mars;
using namespace mars::net;

void onRequest(const HttpRequest& req, HttpResponse* resp) {
  std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
  if (!req.getHeader("Connection").empty()) {
    resp->setCloseConnection(true);
  }
  resp->setStatusCode(HttpResponse::k200Ok);
  resp->setStatusMessage("OK");
  resp->setContentType("text/html");
  resp->addHeader("Server", "Mars");
  std::string now = base::Timestamp::now().toFormattedString();
  resp->setBody("<html><head><title>This is title</title></head>"
                "<body><h1>Hello this is mars website</h1>Now is " + now +
                "</body></html>");
}

int main() {
  EventLoop loop;
  HttpServer server(&loop, InetAddress(9000));
  server.setHttpCallback(onRequest);
  server.start();
  loop.loop();
}
