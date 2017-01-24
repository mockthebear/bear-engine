#ifndef GAMEFILEH
#define GAMEFILEH



#include <string>
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER


/**
 * @brief Class made to access game files like assets or configs
 *
 * On its creation was an class to open files in multi plataform. The was sdlRWops
 * Now this class is just an binding of RWops to make it work with all the engine
 * and path issues between plafaforms... Basically an class to make files open in all
 * suported plataforms.
 * Some point you need to know in:
 * GameFile::Cache
 * resources tweak in ResourceManager
 */
class GameFile{
    public:
        /**
            *LEL, dont waste your time on empty constructor. srsly
            *it just start with 0 on everything
        */
        GameFile();
        /**
            *Bye!
            *It closes eveyrting open!
        */
        ~GameFile();
        /**
            *Open an file
            *Accept resource tweaks
            @param name the path. Or resoruce tweaks "asset:file.bla" Check ResourceFile class
            @param notify print on Console if fails
        */
        GameFile(std::string name,bool notify=false);
        /**
            *Open an file
            *Accept resource tweaks
            @param name the path. Or resoruce tweaks "asset:file.bla" Check ResourceFile class
            @param notify print on Console if fails
        */
        GameFile(const char *name,bool notify=false);
        /**
            *Open an file
            *Accept resource tweaks
            @param name the path. Or resoruce tweaks "asset:file.bla" Check ResourceFile class
            @param notify print on Console if fails
            @return true if sucess
        */
        bool Open(std::string name,bool notify=false);
        /**
            *Close the file. Dont delete the cached stuff
            @return true if sucess
        */
        bool Close();
        /**
            *Delete the cache
            *Dont delete the file
            @return true if sucess
        */
        bool ClearCache();
        /**
            * Check GameFile::Cache
            @return true if the file is open
        */
        bool IsOpen(){return m_filePointer != nullptr;};

        /**
            @return true if there is cached stuff
        */
        bool IsCached(){return m_cache != NULL;};
        /**
            * Read all the file, then cache it.
            @return true if the file is open
        */
        bool Cache();
        /**
            * Return the pointer to the cached data.
            * Its named Unsafe because it returns THE ADERESS, so it foi mess with
            * the data, it will be messes on the whole class.
            @return the aderess to the cached buffer
        */
        char *GetCache_Unsafe(){return m_cache;}; //Get current cache
        /**
            * Copy the cached data, then return it.
            * Note that you <b>need to delete</b> it later.
            @return the aderess to the copy cached buffer
        */
        char *GetCache(); // Copy current cache. Must use delete later
        /**
            * Return the current cached buffer, like GameFile::GetCache_Unsafe
            * But this one simply say to the class that there is no cached content.
            @return the aderess to the current cached buffer
        */
        char *PopCache(); // Return the cached file, and say to the object that is not cached anymore
        //Read
        /**
            * Read one byte at the current position
            * adds +1 to the position
            @return the char
        */
        char ReadByte();
        /**
            * Read one byte at the current position
            * adds +1 to the position
            @return an number
        */
        const char *Read(uint16_t size);

        uint8_t Read8();
        /**
            * Read two bytes and return an uint16_t
            * adds +2 to the position
            @return an number
        */
        uint16_t Read16(){
            char l[2];
            l[0] = Read8();
            l[1] = Read8();
            uint16_t *t = (uint16_t*)l;
            return *t;
        }
        /**
            * Read four bytes and return an uint32_t
            * adds +4 to the position
            @return an number
        */
        uint32_t Read32(){
            char l[4];
            l[0] = Read8();
            l[1] = Read8();
            l[2] = Read8();
            l[3] = Read8();
            uint32_t *t = (uint32_t*)l;
            return *t;
        }
        /**
            Get one line per time
        */
        bool GetLine(std::string &line);

        std::string ReadUntil(char c);
        std::string ReadWord(int count=0,char separator=' ');

        /**
            Return the number
            @param ignoreUntilFind When set to true, it will read all the file from the current point until find any number.
        */
        int GetNumber(bool ignoreUntilFind=false);
        /**
            * Get file size
            @return an number
        */
        uint32_t GetSize(){return m_size;};
        /**
            * Like function ftell. It gives you the current position
            @return an number
        */
        uint32_t Tell(){return m_filePos;};
        /**
            * Like fseek
            @return true if sucess
        */
        bool Seek(uint32_t pos);

        int FindInt();

    private:


        char *m_cache;
        uint32_t m_size;
        uint32_t m_filePos;
        SDL_RWops *m_filePointer;


};


#endif // GAMEFILEH
