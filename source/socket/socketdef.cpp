#include "socketdef.hpp"

#ifndef DISABLE_SOCKET


#ifdef _WIN32
#include <winsock2.h>
#endif // _WIN32


bool BaseSocket::StartSocket(){
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        return false;
    }
    #endif // _WIN32
    return true;
}

#endif
