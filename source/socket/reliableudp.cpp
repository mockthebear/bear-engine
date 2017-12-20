#include "reliableudp.hpp"

bool ReliableUdpClient::StartedEnet = false;

bool ReliableUdpClient::IsConnected(){
    return true;
}

bool ReliableUdpClient::Send(SocketMessage *msg){
    return true;
}
bool ReliableUdpClient::ReceiveBytes(SocketMessage *msg,uint16_t amount){
    return true;
}
bool ReliableUdpClient::Receive(SocketMessage *msg,char breakpad){
    return true;
}

bool ReliableUdpClient::Connect(std::string addr,uint16_t port){
    if (!StartedEnet){
        StartedEnet = enet_initialize()  == 0;
    }
    client = enet_host_create (NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
            14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    return client != nullptr;
}
