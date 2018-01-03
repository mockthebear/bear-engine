#include "socketdef.hpp"
#ifndef DISABLE_SOCKET
#include <enet/enet.h>

class ReliableUdpClient: public SocketClient{
    public:
        ReliableUdpClient():SocketClient(),client(nullptr){};

        bool IsConnected();

        bool Send(SocketMessage *msg);
        bool ReceiveBytes(SocketMessage *msg,uint16_t amount);
        bool Receive(SocketMessage *msg,char breakpad = '\n');

        bool Connect(std::string addr,uint16_t port);
    private:
        static bool StartedEnet;
        ENetHost * client;
};


class ReliableUdpServer: public SocketClient{
    public:
        ReliableUdpServer():SocketClient(),server(nullptr){};

        bool Host(uint16_t port);
    private:
        static bool StartedEnet;
        ENetHost * server;
};

#endif
