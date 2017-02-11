#ifndef RESOURCEFILEHBE
#define RESOURCEFILEHBE
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include "gamefile.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

/**
 * @brief Class to open files .burr. Is an packed asset file
 *
 * When you need to pack all or too many assets in one file, you should use this class
 * creating the .burr with an tool, use this class to open and manage this files.
 * Note that this files will be read only.

 */
class ResourceFile{
    public:
        /**
            *Empty constructor is empty (almost)
        */
        ResourceFile();
        /**
            *Open an file that manages the resource file .burr
            @param dir Where is the file
        */
        ResourceFile(std::string dir);
        /**
            *When you dont call ResourceFile::Close() the destructor call it for you.
        */
        ~ResourceFile();

        /**
            Open an .burr file
            @param dir Use like: "assets:map.burr"
            @return true or false
        */
        bool Open(std::string dir);
        /**
            *Close an current open file
        */
        void Close();
        /**
            Check if some file is open
            @return true or false
        */
        bool IsOpen();
        /**
            *Get the amount of files in the packet
            @return true or false
        */
        int GetFileCount();
        /**
            *Get the name of all files
            @return Vector that contains the name of each file
        */
        std::vector<std::string> ListFiles();
        /**
            Get an file (inside the .burr), as SDL_RWops. Its created a new pointer. Should be destroyed.
            @param name The name of file
            @return SDL_RWops* , NULL when fails
        */
        SDL_RWops* GetFile(std::string name);
        /**
            Get an file (inside the .burr), as SDL_RWops. Its created a new pointer. Should be destroyed.
            @param name The name of file
            @param size Reference int to get the file size
            @return SDL_RWops* , NULL when fails
        */
        SDL_RWops* GetFile(std::string name,int &size);
        /**
            Get an file (inside the .burr), as char*. <b>DONT DELETE IT</b>
            @param name The name of file
            @param size Reference int to get the file size
            @return char* , NULL when fails
        */
        char *GetFileStream(std::string name,int &size);
        /**
            Get size of a given file name (inside the .burr)
            @param name The name of file
            @return 0 when fails
        */
        int GetFileSize(std::string name);
    private:
        GameFile l_file;
        bool m_isCached;
        uint16_t m_header;
        bool m_isOpen;
        char *m_cached;
        uint32_t m_fileSize;
        uint16_t m_fileCount;
        std::unordered_map<std::string,std::tuple<uint32_t,uint32_t>> m_fileData;
};
#endif // RESOURCEFILEHBE
