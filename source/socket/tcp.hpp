#include "socketdef.hpp"


class TcpClient: public SocketClient{
    public:
        TcpClient():SocketClient(){};

        bool IsConnected();

        bool Send(SocketMessage *msg);
        bool ReceiveBytes(SocketMessage *msg,uint16_t amount);
        bool Receive(SocketMessage *msg,char breakpad = '\n');

        bool Connect(std::string addr,uint16_t port);
};
