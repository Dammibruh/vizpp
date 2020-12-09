#include <ixwebsocket/IXWebSocketServer.h>
#include <dammi/logger.hpp>
#include <vector>

namespace vizpp {
    class Server {
        int port = 2000;
        int logged_in_count = 0;
        std::vector<std::string> messages;
        Logger serv_logger;
        public:
        void initServer(){
            ix::WebSocketServer ws_server(this->port);
            auto ws_handler = [&](std::shared_ptr<ix::ConnectionState> conn,
                    ix::WebSocket& ws, const ix::WebSocketMessagePtr& msg){
                switch(msg->type){
                    case ix::WebSocketMessageType::Open:
                        this->logged_in_count++;
                        this->serv_logger.info("new connection etablished\n");
                        this->serv_logger.info("user logged in now are " + std::to_string(this->logged_in_count) + " users\n");
                        break;
                    case ix::WebSocketMessageType::Message:
                        this->messages.push_back(msg->str);
                        if(this->messages.size() > 0){
                            ws.send("last message: " + this->messages[this->messages.size()-1]);
                        }
                        this->messages.push_back(msg->str);
                        break;
                    case ix::WebSocketMessageType::Close:
                        this->logged_in_count--;
                        break;
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
