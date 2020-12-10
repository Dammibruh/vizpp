#include <ixwebsocket/IXWebSocketServer.h>
#include <dammi/logger.hpp>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace vizpp {
    class Server {
        int port = 2000;
        int logged_in_count = 0;
        Logger serv_logger;
        public:
        void initServer(){
            ix::WebSocketServer ws_server(this->port);
            auto ws_handler = [&](std::shared_ptr<ix::ConnectionState> conn,
                    ix::WebSocket& ws, const ix::WebSocketMessagePtr& msg){
                switch(msg->type){
                    case ix::WebSocketMessageType::Open: {
                                                             this->logged_in_count++;
                                                             for(auto&& client: ws_server.getClients()){
                                                                 client->sendBinary("new user joined"); 

                                                             };
                                                             this->serv_logger.info("new connection etablished\n");
                                                             this->serv_logger.info("user logged in now are " + std::to_string(this->logged_in_count) + " users\n");
                                                             break;
                                                         }
                    case ix::WebSocketMessageType::Message: {
                                                                auto req = json::parse(msg->str);
                                                                auto type = req["type"];
                                                                if(type == "message"){
                                                                    for(auto&& client: ws_server.getClients()){
                                                                        if(client.get() != &ws){
                                                                            auto dt = fmt::format("[{}]: {}",
                                                                                    req["username"],
                                                                                    req["message"]);
                                                                            client->sendBinary(dt);
                                                                        }
                                                                    }
                                                                }
                                                                break;
                                                            }
                    case ix::WebSocketMessageType::Close: {
                                                              this->logged_in_count--;
                                                              this->serv_logger.info("A user disconnected\n");
                                                              this->serv_logger.info("user logged in now are " + std::to_string(this->logged_in_count) + " users\n");
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
}
