#ifndef BEUTILS_H
#define BEUTILS_H

#include "../engine/bear.hpp"
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <memory>


namespace utils {


    template<typename... arg> inline std::string format() { return ""; }

    template<typename... arg> inline std::string format(const std::string& txt) { return txt; }

    template<typename... arg> std::string format(const std::string& txt, const arg&... a ) {
        char Buffer[400];
		sprintf(Buffer,txt.c_str(),a...);
        return std::string(Buffer);
    }


    template<typename... arg> std::string format(const std::string& txt,std::string str, const arg&... a ) {
        char Buffer[400];
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

    template< typename T > void bsort( T *a, int n ){
        for( int i=0 ; i<n ; ++i )
        {
            for( int j=i+1 ; j<n ; ++j )
            {
                if( a[j].rot < a[i].rot )
                {
                    T temp = a[i] ;
                    a[i] = a[j] ;
                    a[j] = temp ;
                }
            }
        }
    }

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
        if (x < 0 || x >= max){
            return false;
        }
        return true;
    }
    inline bool IsInPosOk(int x,int y,int max){
        if ( IsInRange(x, max) && IsInRange(y, max)){
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
    char ReadChar(std::string &str);



    template<typename T> class Mat3{
        public:
            Mat3(){
                Data = nullptr;
            };
            Mat3(int sizeX_,int sizeY_,int sizeZ_=1){
                sizeX = sizeX_;
                sizeY = sizeY_;
                sizeZ = sizeZ_;
                TotalSize = sizeZ * sizeY * sizeX;
                Data = new T[ TotalSize ];

            };
            T &at(int x,int y,int z){
                if (Data == nullptr){
                    bear::out << "Is null\n";
                }
                return Data[ x + (y*sizeX) + (z*sizeX*sizeY) ];
            }
            void erase(){
                if (Data){
                    delete [] Data;
                    Data = nullptr;
                }
            }
        private:
            uint32_t sizeX;
            uint32_t sizeY;
            uint32_t sizeZ;
            uint32_t TotalSize;
            T *Data;
    };
    template<int N> struct expander{
        template<typename... Args>
            static uint32_t expand(uint32_t sMax,uint32_t *addr, uint32_t v1, const Args&... args) {
                addr[N] = v1;
                return v1 * expander<N-1>::expand(sMax,addr,args...);
        }
        static uint32_t expandOffset(uint32_t *addr) {
                return addr[N]*expander<N-1>::expandOffset(addr);
        }
        template<typename... Args>
            static uint32_t expandAdderess(uint32_t *addr, uint32_t v1, const Args&... args) {
                if (addr[N] <= v1){
                    bear::out << "[Warning] Acessing coordinate "<<N<<" wich has size of "<<addr[N]<<" and requested position "<<v1<<"\n";
                }
                int first =  expander<N-1>::expandOffset(addr) * v1;
                int second = expander<N-1>::expandAdderess(addr,args...);
                return first+ second ;
        }
    };
    template<> struct expander <0> {
        template<typename... Args>
            static uint32_t expand(uint32_t sMax,uint32_t *addr) {
                return 1;
        }
        static uint32_t expandAdderess(uint32_t *addr) {
			(void)addr;
			return 0;
        }
        static uint32_t expandOffset(uint32_t *addr) {
			(void)addr;
			return 1;
        }
    };

    template<typename T,int N> class MatN{
        public:
            MatN(){
                Data = nullptr;
            };
            template <typename ... Types> MatN(Types ... args):MatN(){
				if ((sizeof...(Types)) != N){
                    bear::out << "wrong argument size1\n";
                    return;
                }
                m_isDynamic = true;
                m_totalSize = expander<sizeof...(Types)>::expand(N,coordSizes,args...);
				try {
					Data = new T[ m_totalSize ];
				}
				catch (const std::bad_alloc& e) {
					bear::out << "Allocation failed: " << e.what() << '\n';
				}
                if (!Data){
                    bear::out << "Cannot allocate an matrix sized with "<<(m_totalSize*sizeof(T)) << " bytes\n";
                }
            };

            template <typename ... Types> MatN(T * dat, Types ... args):MatN(){
				if ((sizeof...(Types)) != N){
                    bear::out << "wrong argument size1\n";
                    return;
                }
                m_isDynamic = false;
                m_totalSize = expander<sizeof...(Types)>::expand(N,coordSizes,args...);
                Data = dat;
                if (!Data){
                    bear::out << "Passed a null data!\n";
                }
            };

            template <typename ... Types> MatN(T **dat, Types ... args):MatN(){
				if ((sizeof...(Types)) != N){
                    bear::out << "wrong argument size1\n";
                    return;
                }
                m_isDynamic = false;
                m_totalSize = expander<sizeof...(Types)>::expand(N,coordSizes,args...);
                Data = (T*)dat;
                if (!Data){
                    bear::out << "Passed a null data!\n";
                }
            };

            template <typename ... Types> T &at(Types ... args){
                if ((sizeof...(Types)) != N){
                    bear::out << "wrong argument size 2. Got "<<(sizeof...(Types))<<" expected " << N <<"\n";
                }
                int addrOffset = expander<sizeof...(Types)>::expandAdderess(coordSizes,args...);
                return Data[ addrOffset ];
            }
            void erase(){
                if (Data && m_isDynamic){
                    delete [] Data;
                }
            }

        private:
            uint32_t coordSizes[N+1];
            uint32_t m_totalSize;
            bool m_isDynamic;
            T *Data;
    };


}
class PlainRand{
    public:
        PlainRand():Size(1),last(-1),SelectorMap(){};
        PlainRand(int n):PlainRand(){
            SelectorMap.clear();
            for (int i=0;i<n;i++){
                SelectorMap[i] = 0;
            }
            last = -1;
            Size = n;
        }
        int Generate(){
            int MinNum = 0xffffff;
            for (int i=0;i<Size;i++){
                if (SelectorMap[i] < MinNum){
                    MinNum = SelectorMap[i];
                }
            }
            while (1){
                int Selected;
                do{
                    Selected = rand()%Size;
                } while (Selected == last);

                if (SelectorMap[Selected] == MinNum){
                    last = Selected;
                    SelectorMap[Selected]++;
                    return Selected;
                }
            }
        }
    private:
        int Size;
        int last;
        std::map<int,int> SelectorMap;
};
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

template<typename T> class GenericStaticIterator {
public:
    GenericStaticIterator(T **vector_, int size) {
        T ** aux = new T*[size];
        memcpy(aux, vector_, sizeof(T **) * size);

        ptr = std::shared_ptr <T*>(aux, std::default_delete<T*[]>());
        sz = size;
    };
    T** begin() {
        return &ptr.get()[0];
    }
    /**
        Used to be the iterator on c++11
    */
    T** end() {
        return &ptr.get()[sz];
    }
private:
    std::shared_ptr<T*> ptr;
    int sz;
};




#endif
