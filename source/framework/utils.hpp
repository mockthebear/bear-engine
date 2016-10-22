#ifndef BEUTILS_H
#define BEUTILS_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>


namespace utils {


    template<typename... arg> inline std::string format() { return ""; }

    template<typename... arg> inline std::string format(const std::string& txt) { return txt; }

    template<typename... arg> std::string format(const std::string& txt, const arg&... a ) {
        char Buffer[100];
        sprintf(Buffer,txt.c_str(),a...);
        return std::string(Buffer);
    }


    template<typename... arg> std::string format(const std::string& txt,std::string str, const arg&... a ) {
        char Buffer[100];
        sprintf(Buffer,txt.c_str(),str.c_str(),a...);
        return std::string(Buffer);
    }

    template<class T,typename... arg> void pack(std::vector<T> &vec){};
    template<class T,typename... arg> void pack(std::vector<T> &vec,T head){
        vec.emplace_back(head);
    };
    template<class T,typename... arg> void pack(std::vector<T> &vec,T head,const arg&... tail){
        vec.emplace_back(head);
        pack(vec,tail...);
    };

    template<class T,typename... arg> T choose(){return T();};
    template<class T,typename... arg> T choose(T head,const arg&... tail){
        std::vector<T> vec;
        vec.emplace_back(head);
        pack(vec,tail...);
        T ret = vec.at(rand()%vec.size());
        return ret;

    };
    char GetByte (char *str,int pos=0);
    uint8_t  GetU8 (char *str,int pos = 0);
    uint16_t GetU16(char *str,int pos = 0);
    uint32_t GetU24(char *str,int pos = 0);
    uint32_t GetU32(char *str,int pos = 0);
    uint64_t GetU64(char *str,int pos = 0);
    uint8_t  GetU8c (char *str,int &pos);
    uint16_t GetU16c(char *str,int &pos);
    uint32_t GetU24c(char *str,int &pos);
    uint32_t GetU32c(char *str,int &pos);
    uint64_t GetU64c(char *str,int &pos);







    std::string methodName(const std::string& prettyFunction);

    inline bool IsInRange(int x,int max){
        if (x < 0 or x >= max){
            return false;
        }
        return true;
    }
    inline bool IsInPosOk(int x,int y,int max){
        if ( IsInRange(x, max) and IsInRange(y, max)){
            return true;
        }
        return false;
    }

    uint32_t GetAround(uint32_t**matrix ,int x,int y,int sx,int sy);
    bool IsInLimits(int x,int y,int sx,int sy);


    /*
        String works
    */

    int GetNumber(std::string &str,bool ignoreUntilFind=false);
    int TrimString(std::string &str,char tocut=' ');
    std::string ReadWord(std::string &str,int n=1,char separator=' ');
    std::string ReadUntil(std::string &str,char rd);
    std::string ReadUntil(std::string &str,std::string until);
    std::string GetLine(std::string &str);

}

template<typename T> class GenericIterator{
    public:
        GenericIterator(T **vector_,int size){
            vector = vector_;
            sz = size;
        }
        T** begin(){
            return &vector[0];
        }
        /**
            Used to be the iterator on c++11
        */
        T** end(){
            return &vector[sz];
        }
    private:
        T **vector;
        int sz;
};


#endif
