#include "reliableudp.hpp"
#include "../engine/bear.hpp"

#if !defined(DISABLE_SOCKET)
bool ReliableUdpClient::StartedEnet = false;

bool ReliableUdpClient::IsConnected(){
    return true;
}

ReliableUdpClient::~ReliableUdpClient(){
    Close();
}

void ReliableUdpClient::Close(){
    if (peer){
        enet_peer_disconnect(peer,0);
        enet_peer_reset(peer);
        peer = nullptr;
    }
    if (client){
        enet_host_destroy(client);
        client = nullptr;
    }
}


void ReliableUdpClient::Update(float dt){
    if (!client || !peer){
        return;
    }
    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service (client, &event, 0) > 0){
        switch (event.type){
            case ENET_EVENT_TYPE_NONE:{
                break;
            }
            case ENET_EVENT_TYPE_CONNECT:{
                bear::out << "cuneqtado\n";
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:{
                SocketMessage msg;
                msg.SetStream((char*)event.packet -> data,event.packet -> dataLength);
                m_messages.push(msg);
                enet_packet_destroy (event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:{
                bear::out << "Disconected bro :c \n";
                event.peer -> data = NULL;
                break;
            }
        }
    }
}


bool ReliableUdpClient::Send(SocketMessage *msg){
    if (peer){
        ENetPacket * packet = enet_packet_create (msg->GetStream(),  msg->GetMessageSize()+1, sendMode);
        enet_peer_send(peer, 0, packet);
        return true;
    }else{
        return false;
    }
}
bool ReliableUdpClient::ReceiveBytes(SocketMessage *msg,uint16_t amount){
    return true;
}

bool ReliableUdpClient::Receive(SocketMessage *msg,char breakpad){
    if (peer && m_messages.size() > 0){
        (*msg) = SocketMessage(m_messages.front());
        m_messages.pop();
        return true;
    }
    return false;
}

bool ReliableUdpClient::Start(){
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

bool ReliableUdpClient::Connect(std::string addr,uint16_t port,int waitTime){
    if (!client){
        Start();
    }
    ENetAddress address;
    enet_address_set_host (&address, addr.c_str());
    address.port = port;
    peer = enet_host_connect (client, &address, 2, 0);
    if (enet_host_service (client, &event, waitTime)  <= 0){
        enet_peer_reset (peer);
        return false;
    }
    return event.type == ENET_EVENT_TYPE_CONNECT;
}

void ReliableUdpServer::Close(){
    for (uint64_t i=0;i<m_lastPid;i++){
        if (peers[i]){
            enet_peer_disconnect(peers[i],0);
            enet_peer_reset(peers[i]);
            peers[i] = nullptr;
        }
    }
    if (server){
        enet_host_destroy(server);
        server = 0;
    }
    m_lastPid = 0;
}

ReliableUdpServer::~ReliableUdpServer(){
    Close();
}


bool ReliableUdpServer::Bind(uint16_t port){
    if (!ReliableUdpClient::StartedEnet){
        ReliableUdpClient::StartedEnet = enet_initialize()  == 0;
    }
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    server = enet_host_create (& address /* the address to bind the server host to */,
                             32      /* allow up to 32 clients and/or outgoing connections */,
                              2      /* allow up to 2 channels to be used, 0 and 1 */,
                              0      /* assume any amount of incoming bandwidth */,
                              0      /* assume any amount of outgoing bandwidth */);
    return server != nullptr;
}
void ReliableUdpServer::Update(float dt){
    if (!server)
        return;
    while (enet_host_service (server, &event, 0) > 0){
        switch (event.type){
            case ENET_EVENT_TYPE_NONE:{
                break;
            }
            case ENET_EVENT_TYPE_CONNECT:{
                event.peer->data = (void*)m_lastPid;
                peers[m_lastPid] = event.peer;
                peerIds.emplace_back(m_lastPid);
                printf ("%d connect on server. from : %x\n", (int)m_lastPid,event.peer, (int)event.peer -> address.host);
                m_lastPid++;
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:{
                SocketMessage msg;
                uint32_t pid = (uint64_t)event.peer -> data;
                msg.SetStream((char*)event.packet -> data,event.packet -> dataLength);
                m_messages[pid].push(msg);
                enet_packet_destroy (event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:{
                printf ("%d disconnected.\n", (int)event.peer -> data);
                peerIds.remove(m_lastPid);
            }
        }
    }
}
bool ReliableUdpServer::Receive(SocketMessage *msg,int pid){
    ENetPeer *peer = peers[pid];
    if (peer && m_messages[pid].size() > 0){
        (*msg) = SocketMessage(m_messages[pid].front());
        m_messages[pid].pop();
        return true;
    }
    return false;
}

bool ReliableUdpServer::Send(SocketMessage *msg,int pid){
    if (pid == -1){
        for (uint64_t i=0;i<m_lastPid;i++){
            Send(msg,i);
        }
        return true;
    }else{
        ENetPeer *peer = peers[pid];
        if (peer){
            ENetPacket * packet = enet_packet_create (msg->GetStream(),  msg->GetMessageSize()+1, sendMode);
            enet_peer_send(peer, 0, packet);
            return true;
        }else{
            return false;
        }
    }
}



#endif
