#include "socketdef.hpp"
#ifndef DISABLE_SOCKET
#pragma once
#include <enet/enet.h>
#include <queue>

class ReliableUdpClient: public SocketClient{
    public:
        ReliableUdpClient():SocketClient(),client(nullptr),peer(nullptr){};
        ~ReliableUdpClient();
        bool IsConnected();

        void Update(float dt);

        bool Send(SocketMessage *msg);
        bool ReceiveBytes(SocketMessage *msg,uint16_t amount);
        bool Receive(SocketMessage *msg,char breakpad = '\n');

        bool Start();
        bool Connect(std::string addr,uint16_t port,int wait=1000);
        static bool StartedEnet;

        void Close();
    private:


        ENetHost * client;
        ENetPeer *peer;
        ENetEvent event;
        std::queue<SocketMessage> m_messages;
};


class ReliableUdpServer: public SocketHost{
    public:
        ~ReliableUdpServer();
        ReliableUdpServer():SocketHost(),server(nullptr){for (int i=0;i<100;i++) peers[i] = nullptr;};
        bool Bind(uint16_t port);
        void Update(float dt);
        bool Receive(SocketMessage *msg,int pid);
        bool Send(SocketMessage *msg,int pid);

        void Close();
    private:

        ENetHost *server;
        ENetEvent event;
        ENetPeer *peers[100];
};

#endif
