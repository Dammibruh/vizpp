#include <functional>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
namespace vizpp {
class Client {
  const std::string &url = "ws://localhost:2000";
  const std::string &username;
  ix::WebSocket conn;

public:
  Client(const std::string &url, const std::string &username = "guest")
      : url(url), username(username) {
    ix::WebSocketHttpHeaders headers;
    headers["username"] = username;
    conn.setExtraHeaders(headers);
  }
  void connect() {
    this->conn.setUrl(this->url);
    auto connHandler = [&](const ix::WebSocketMessagePtr &msg) {
      switch (msg->type) {
      case ix::WebSocketMessageType::Open:
        this->on_conn_open();
        break;
      case ix::WebSocketMessageType::Message:
        this->on_message(std::move(msg->str));
        break;
      case ix::WebSocketMessageType::Close:
        this->on_conn_close();
        break;
      }
    };
    this->conn.setOnMessageCallback(connHandler);
    this->conn.start();
  }
  void sendMessage(const std::string &msg) {
    json data;
    data["type"] = "message";
    data["message"] = msg;
    this->conn.send(data.dump());
  }
  std::function<void(const std::string &msg)> on_message;
  std::function<void()> on_conn_open;
  std::function<void()> on_conn_close;
};

} // namespace vizpp
