#include "./src/client.hpp"
#include <iostream>


int main(){
    std::string name;
    bool is_connected;
    std::cout << "name > ";
    std::getline(std::cin, name);
    vizpp::Client client("ws://localhost:2000", name);
    client.on_conn_open = [&](){
        std::cout << "connected to server\n";
    };
    client.on_conn_close = [&](){
        std::cout << "connection lost\n";
        is_connected = false;
    };
    client.on_message = [&](const std::string& msg){
        std::cout << msg << "\n";
    };
    client.connect();
    is_connected = true;
    while(is_connected){
        std::string msg;
        std::getline(std::cin, msg);
        client.sendMessage(msg);
    }
}
