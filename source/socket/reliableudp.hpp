#include "socketdef.hpp"
#ifndef DISABLE_SOCKET
#pragma once
#include <enet/enet.h>
#include <queue>
#include <list>

class ReliableUdpClient: public SocketClient{
    public:
        ReliableUdpClient():SocketClient(),sendMode(ENET_PACKET_FLAG_RELIABLE),client(nullptr),peer(nullptr){};
        ~ReliableUdpClient();
        bool IsConnected();

        void Update(float dt);

        bool Send(SocketMessage *msg);
        bool Send(SocketMessage msg){
            static SocketMessage svd;
            svd = msg;
            return Send(&svd);
        };
        bool ReceiveBytes(SocketMessage *msg,uint16_t amount);
        bool Receive(SocketMessage *msg,char breakpad = '\n');

        bool Start();
        bool Connect(std::string addr,uint16_t port,int wait=1000);
        static bool StartedEnet;

        void SetSendMode(ENetPacketFlag mode){ sendMode = mode;};

        void Close();
    private:

        ENetPacketFlag sendMode;
        ENetHost * client;
        ENetPeer *peer;
        ENetEvent event;
        std::queue<SocketMessage> m_messages;
};


class ReliableUdpServer: public SocketHost{
    public:
        ~ReliableUdpServer();
        ReliableUdpServer():SocketHost(),sendMode(ENET_PACKET_FLAG_RELIABLE),server(nullptr){for (int i=0;i<100;i++) peers[i] = nullptr;};
        bool Bind(uint16_t port);
        void Update(float dt);
        bool Receive(SocketMessage *msg,int pid);
        bool Send(SocketMessage *msg,int pid);
        void Close();
        std::list<int> GetPeers(){ return peerIds;};
        int GetPeersOnline(){ return peerIds.size();};
        void SetSendMode(ENetPacketFlag mode){ sendMode = mode;};
    private:
        ENetPacketFlag sendMode;
        std::list<int> peerIds;
        ENetHost *server;
        ENetEvent event;
        ENetPeer *peers[100];
};

#endif
