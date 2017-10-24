#ifndef USERFILEH
#define USERFILEH



#include <string>
#include <stdlib.h>
#include "../settings/definitions.hpp"
#include "../framework/utils.hpp"
#include SDL_LIB_HEADER


/**
 * @brief Class used to open files to write
 *
 */
class UserFile{
    public:
        UserFile();
        bool Open(std::string name);
        bool Close();
        uint32_t Write(std::string str);
        template<typename... arg>uint32_t Printf(const char *s,const arg&... a){
            std::string str = utils::format(s,a...);
            return Write(str);
        }
        uint32_t Write(char *str,int size);
        uint32_t Write8(char c);
        uint32_t WriteU8(uint8_t c);
        uint32_t WriteU16(uint16_t c);
        uint32_t WriteU32(uint32_t c);
        uint32_t WriteU64(uint64_t c);

        std::string GetFilePath();
        UserFile& operator <<(std::string str){
            Write(str);
            return *this;
        }
        UserFile& operator <<(char c){
            Write8(c);
            return *this;
        }
    private:
        void fputc(char c,SDL_RWops * fptr2){
            const char c2 = c;;
            SDL_RWwrite(fptr,&c2,1,1);
        };
        uint32_t writePos;
        std::string fname;
        SDL_RWops *fptr;
};

#endif
