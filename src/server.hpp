#include <dammi/logger.hpp>
#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>
#include <vector>

/*
 * main Server class
 *
 */

using json = nlohmann::json;

namespace vizpp {
struct User {
  std::string_view name;
  int id;
};
class Server {
  unsigned int port;
  unsigned int logged_in_count = 0;
  Logger serv_logger;
  std::vector<User> users;

public:
  Server(unsigned int port = 2000) : port(port) {}
  void initServer() {
    ix::WebSocketServer ws_server(this->port);
    auto ws_handler = [&](std::shared_ptr<ix::ConnectionState> conn,
                          ix::WebSocket &ws,
                          const ix::WebSocketMessagePtr &msg) {
      switch (msg->type) {
      case ix::WebSocketMessageType::Open: {
        this->logged_in_count++;
        for (auto &&client : ws_server.getClients()) {
          client->sendBinary(fmt::format("{} has joined",msg->openInfo.headers["username"]));
        };
        this->users.push_back(User{.name = msg->openInfo.headers["username"], .id = std::stoi(conn->getId())});
        this->serv_logger.info("new connection etablished\n");
        this->serv_logger.info("user logged in now are " +
                               std::to_string(this->logged_in_count) +
                               " users\n");
        break;
      }
      case ix::WebSocketMessageType::Message: {
        auto req = json::parse(msg->str);
        auto type = req["type"];
        if (type == "message") {
          for (auto &&client : ws_server.getClients()) {
            if (client.get() != &ws) {
              auto dt =
                  fmt::format("[{} | {}]: {}", req["username"], conn->getId(), req["message"]);
              client->sendBinary(dt);
            }
          }
        }
        break;
      }
      case ix::WebSocketMessageType::Close: {
        this->logged_in_count--;
        this->serv_logger.info(fmt::format("{} has disconnected\n", msg->openInfo.headers["username"]));
        this->serv_logger.info("user logged in now are " +
                               std::to_string(this->logged_in_count) +
                               " users\n");
        break;
      }
      }
    };
    ws_server.setOnClientMessageCallback(ws_handler);
    this->serv_logger.info("server stared\n");
    ws_server.listen();
    ws_server.start();
    ws_server.wait();
  }
};
} // namespace vizpp
