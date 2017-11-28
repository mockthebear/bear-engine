#include "tcp.hpp"
#ifdef _WIN32
#include <winsock.h>
#else
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#endif // _WIN32

bool TcpClient::Connect(std::string addr,uint16_t port){
    if (IsConnected()){
        return false;
    }
    if (m_SocketHandler == -1){
        m_SocketHandler = socket(AF_INET, SOCK_STREAM, 0);
    }

    auto server = gethostbyname(addr.c_str());
    bzero((char *) &m_serverAddr, sizeof(m_serverAddr));

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    bcopy((char *)server->h_addr, (char *)&m_serverAddr.sin_addr.s_addr, server->h_length);

    if (connect(m_SocketHandler, (struct sockaddr*) &m_serverAddr, sizeof(m_serverAddr)) < 0){
        return false;
    }

    return true;
}

bool TcpClient::IsConnected(){
    if (m_SocketHandler == -1){
        return false;
    }
    int error_code;
    socklen_t error_code_size = sizeof(error_code);
    int ret = getsockopt(m_SocketHandler, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
    if (ret != 0){
        return false;
    }
    if (error_code == 0){
        return true;
    }
    return false;
}

bool TcpClient::Send(SocketMessage *msg){
    if (!IsConnected() || msg == nullptr){
        return false;
    }
    uint16_t size;
    char *buffer = msg->GetStream(size);
    int ret = send(m_SocketHandler,buffer,size,0);
    return ret == 0;
}

bool TcpClient::ReceiveBytes(SocketMessage *msg,uint16_t amount){
    if (!IsConnected() || msg == nullptr){
        return false;
    }
    uint16_t size;
    uint16_t bytesRead = 0;
    char *buffer = msg->GetStream(size);
    uint16_t leftSize = recv(m_SocketHandler,buffer,amount,0);
    msg->SetSize(amount);
    return true;
}

bool TcpClient::Receive(SocketMessage *msg,char breakpad){
    if (!IsConnected() || msg == nullptr){
        return false;
    }
    uint16_t size;
    uint16_t bytesRead = 0;
    char *buffer = msg->GetStream(size);
    size = msg->GetMaxBuffer();


    while (true){
        uint16_t leftSize = recv(m_SocketHandler,buffer + bytesRead,sizeof(char),0);
        if (buffer[bytesRead] == breakpad){
            break;
        }
        if (leftSize == 0){
           break;
        }
        bytesRead++;
    }
    buffer[bytesRead] = '\0';
    msg->SetSize(bytesRead);
    return true;
}
