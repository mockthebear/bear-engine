#ifndef RESOURCEMANAGERHBE
#define RESOURCEMANAGERHBE
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include <unordered_map>
#include "resourcefiles.hpp"


/**
 * @brief Singleton class to manage ResourceFile around the game
 *
 * Using this class you may access the resources from .burr files from all game scope.
 * Check ResourceFile class
 * This class holds an thing called </b>Resource Tweak<b>
 * When ResourceManager::Load is called, its second parameter is an alias
 * Lets say you set your alias to "magic"
 * And there is a file called "happened.png" inside the packed one.
 * You can refeer this file in class Sprite like this:
 * "magic:happened.png" on its creator.
 * You have a list of what classes that load some file support Resource Tweak:
 * Sprite
 * jeej
 */


class ResourceManager{
    public:
        /**
            *LEL, dont waste your time on this constructor. srsly
        */
        ResourceManager();
        /**
            *Meyer's singleton.
            *@code
            SDL_RWops *r = ResourceManager::GetInstance().GetFile("assets:text.txt");
            @endcode
            @return Reference to ResourceManager
        */
        static ResourceManager& GetInstance();
        /**
            *Here you can load an .burr file, then set an alias to this.
            * Lets say, you load "data/images/objects/moveable/yellow/coins.burr"
            * You will have every time that you load or search for files in this pack, to write the name
            * better just set an alias like "ycoins";
            *@code
            ResourceManager::GetInstance().Load("data/images/objects/moveable/yellow/coins.burr","ycoins");
            SDL_RWops *r = ResourceManager::GetInstance().GetFile("ycoins","coin.png");
            @endcode
            *When alias not set, the current alias will be the path name
            @param file The path for the packed file
            @param alias An alias to the name
            @return True if loaded
        */
        bool Load(std::string file,std::string alias="");
        /**
            *Get an file (inside the .burr), as SDL_RWops. Its created a new pointer. Should be destroyed.
            *Use the alias to say what packed file is
            @param assetAlias The alias
            @param fileName The file name
            @return SDL_RWops* , NULL when fails
        */
        SDL_RWops* GetFile(std::string assetAlias,std::string fileName);
        /**
            *Get an file (inside the .burr), as SDL_RWops. Its created a new pointer. Should be destroyed.
            *Use the alias to say what packed file is
            *On most cases, you can use this, also work on Sprite class.
            *Use like this:
            *@code
            ResourceManager::GetInstance().Load("data/images/objects/moveable/yellow/coins.burr","ycoins");
            SDL_RWops *r = ResourceManager::GetInstance().GetFile("ycoins:coin.png");
            @endcode
            *This is an example of Resource Tweak
            *The engine will explode the string on the ":" character
            *The first one will be the asset alias, the second one, the file.
            @param fileName alias + filename like "alias:filename"
            @return SDL_RWops* , NULL when fails
        */
        SDL_RWops* GetFile(std::string file);
        /**
            *Erase an asset
            @param assetAlias the alias
            @return true if sucess
        */
        bool Erase(std::string assetAlias);

    private:
        static std::unordered_map<std::string, ResourceFile*> resources;
};
#endif // RESOURCEMANAGERHBE
