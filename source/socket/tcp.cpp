#include "tcp.hpp"
#ifndef DISABLE_SOCKET
#include "../engine/bear.hpp"
#include "../framework/utils.hpp"
#include <stdio.h>
#include <strings.h>
#include <string.h>
#ifdef _WIN32
    #ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
    #endif // _WIN32_WINNT

    #define _WIN32_WINNT 0x501

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
    #endif // INVALID_SOCKET
#endif // _WIN32
#include <stdio.h>

bool TcpClient::Connect(std::string addr,uint16_t port){
    if (IsConnected()){
        return false;
    }

    #ifdef _WIN32
    if (m_SocketHandler == INVALID_SOCKET){
        m_SocketHandler = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
    #else
    if (m_SocketHandler == -1){
    #endif
        m_SocketHandler = socket(AF_INET, SOCK_STREAM, 0);
    }


    #ifdef _WIN32
    struct addrinfo *result = NULL,
                    hints;

    memset(&hints, 0, sizeof(hints));


    getaddrinfo(addr.c_str(), utils::format("%d",port).c_str(), &hints, &result);


    if (connect(m_SocketHandler, result->ai_addr , (int)result->ai_addrlen) < 0){
        #ifdef _WIN32
        closesocket(m_SocketHandler);
        #endif // _WIN32
        return false;
    }

    #else



    auto server = gethostbyname(addr.c_str());

    memset(&m_serverAddr, 0, sizeof(m_serverAddr));

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);

    memcpy(server->h_addr,&m_serverAddr.sin_addr.s_addr,server->h_length);


    if (connect(m_SocketHandler, (struct sockaddr*) &m_serverAddr, sizeof(m_serverAddr)) < 0){
        return false;
    }
    #endif // _WIN32
    return true;
}

bool TcpClient::IsConnected(){
    if (m_SocketHandler == INVALID_SOCKET){
        return false;
    }
    int error_code;
    socklen_t error_code_size = sizeof(error_code);
    #ifdef _WIN32
    int ret = getsockopt(m_SocketHandler, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_code_size);
    #else
    int ret = getsockopt(m_SocketHandler, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
    #endif
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
    char *buffer = msg->GetStream(size);
    recv(m_SocketHandler,buffer,amount,0);
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

#endif
