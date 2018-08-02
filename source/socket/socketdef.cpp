#include "socketdef.hpp"

#include "../settings/definitions.hpp"
#if !defined(DISABLE_SOCKET)


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
#else

bool BaseSocket::StartSocket(){
    return false;
}
#endif
