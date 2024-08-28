#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include "m_netlib/Net/EventLoop.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace mars;
using namespace mars::net;

void onRequest(const HttpRequest& req, HttpResponse* resp) {
    std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;

    // 处理不同路径
    if (req.path() == "/picture") {
        if(!req.getHeader("Connection").empty()) {
            resp->setCloseConnection(true);
        }

        // 处理图片请求
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setContentType("image/png");

        std::ifstream file("/home/mars/图片/屏幕截图/屏幕截图_20240826_215157.png", std::ios::binary);
        if (file) {
            resp->setStatusMessage("OK");
            std::ostringstream oss;
            oss << file.rdbuf();
            resp->setBody(oss.str());
        } else {
            resp->setStatusCode(HttpResponse::k404NotFound);
            resp->setStatusMessage("Not Found");
        }
    } else if (req.path() == "/") {
        // 处理主页请求
        if (!req.getHeader("Connection").empty()) {
            resp->setCloseConnection(true);
        }
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "Mars");

        std::string now = base::Timestamp::now().toFormattedString();
        std::string body = R"(
            <html>
            <head>
              <title>Mars Website</title>
              <style>
                body {
                  font-family: Arial, sans-serif;
                  background-color: #f4f4f4;
                  margin: 0;
                  padding: 0;
                  display: flex;
                  justify-content: center;
                  align-items: center;
                  height: 100vh;
                  color: #333;
                }
                .container {
                  text-align: center;
                  background: white;
                  padding: 20px;
                  border-radius: 10px;
                  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                }
                h1 {
                  color: #007BFF;
                }
                p {
                  font-size: 18px;
                }
                .time {
                  font-weight: bold;
                  margin-top: 20px;
                  font-size: 20px;
                }
                .image {
                  margin-top: 20px;
                }
              </style>
            </head>
            <body>
              <div class="container">
                <h1>Welcome to Mars Website</h1>
                <p>This is a beautiful and modern webpage.</p>
                <div class="time">Now is: )" + now + R"(</div>
              </div>
            </body>
            </html>
        )";

        resp->setBody(body);
    } else {
        // 处理其他未知路径的请求
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}

int main() {
  EventLoop loop;
  HttpServer server(&loop, InetAddress(9000));
  server.setHttpCallback(onRequest);
  server.start();
  loop.loop();
}
