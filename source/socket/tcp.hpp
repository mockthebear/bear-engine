#include "socketdef.hpp"

#if !defined(DISABLE_SOCKET)

class TcpClient: public SocketClient{
    public:
        TcpClient():SocketClient(){};

        bool IsConnected();
        void Update(float dt);
        bool Send(SocketMessage *msg);
        bool ReceiveBytes(SocketMessage *msg,uint16_t amount);
        bool Receive(SocketMessage *msg,char breakpad = '\n');

        bool Connect(std::string addr,uint16_t port,int duration=0);
};

#endif
