#ifndef __EMSCRIPTEN__
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
typedef sockaddr_in SocketAddr;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
typedef struct sockaddr_in SocketAddr;
#endif // _WIN32


template <int MsgSize=1024> class SocketMessage_{
    public:
        SocketMessage_():m_pointer(0),m_readPointer(0){
            for (int i=0;i<MsgSize;i++){
                StaticStream[i] = 0;
            }
        };
        uint32_t GetMaxBuffer(){
            return MsgSize;
        }
        void SetSize(uint16_t size){
            m_pointer = size;
            m_readPointer = 0;

        };
        void Clear(){
            m_readPointer = 0;
            m_pointer = 0;
        }
        bool SetStream(char *stream,uint16_t size=0){
            if (size >= MsgSize){
                return false;
            }
            if (size == 0){
                size = strlen(stream);
            }
            memcpy(StaticStream,stream,size);
            m_pointer = size;
            m_readPointer = 0;
            return true;
        };

        char *GetStream(uint16_t &size){
            size = m_pointer;
            return StaticStream;
        };

        char *GetStream(){
            return StaticStream;
        };


        bool Skip(uint16_t val=1){
            if (m_readPointer+val >= MsgSize){
                return false;
            }
            m_readPointer += val;
            return true;
        }

        char ReadByte(){
            char ret = StaticStream[m_readPointer];
            m_readPointer++;
            return ret;
        }

        bool AddByte(char b){
            if (m_pointer >= MsgSize){
                return false;
            }
            StaticStream[m_pointer] = b;
            m_pointer++;
            return true;
        }

        template<typename T>bool Add(T obj){
            if (m_pointer+sizeof(T) >= MsgSize){
                return false;
            }
            char addr = (T*)StaticStream[m_pointer];
            m_pointer += sizeof(T);
            memcpy(addr,&obj,sizeof(T));
            return true;
        }

        template<typename T>T Get(){
            if (m_readPointer+sizeof(T) >= m_pointer){
                return T();
            }
            T ret = *(T*)StaticStream[m_readPointer];
            m_readPointer += sizeof(T);
            return ret;
        }


        uint16_t GetMessageSize(){
            return m_pointer;
        }
        uint16_t Seek(uint16_t pos = 0){
            uint16_t myPos = m_readPointer;
            m_readPointer = pos;
            return Seek;
        }
    private:
        uint16_t m_pointer,m_readPointer;
        char StaticStream[MsgSize];
};

typedef SocketMessage_<1024> SocketMessage;


class SocketClient{
    public:
        SocketClient():m_SocketHandler(-1){};
        virtual bool Connect(std::string addr,uint16_t port) = 0;

        virtual bool Receive(SocketMessage *msg,char breakpad = '\n') = 0;
        virtual bool ReceiveBytes(SocketMessage *msg,uint16_t amount) = 0;
        virtual bool Send(SocketMessage *msg) = 0;
        virtual bool IsConnected(){return false;};
    protected:
        int m_SocketHandler;
        SocketAddr m_serverAddr;

};

class SocketHost{
    public:
        SocketHost();
        virtual bool Open(std::string addr,uint16_t port) = 0;

        virtual bool Receive(SocketMessage *msg) = 0;
        virtual bool Send(SocketMessage *msg) = 0;
    protected:
        uint32_t clients;
};



#endif // __EMSCRIPTEN__
