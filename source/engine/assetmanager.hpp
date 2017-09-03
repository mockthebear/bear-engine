#pragma once
#include <memory>
#include <unordered_map>
#include "sprite.hpp"
#include "../sound/sound.hpp"
#include "../performance/console.hpp"
#include <iostream>

/**
    @brief Asset manager class
*/

class AssetMannager{
    public:
        /**
            Empty constructor
        */
        AssetMannager();
        /**
            Empty destructor
            <b>DO NOT ERASE ASSETS</b>
        */
        ~AssetMannager();
        /**
            Erase all assets
        */
        bool erase();



        /**
            * Get an list of all assets with the given T name
            @return an std::string vector
        */
        template<typename T> std::vector<std::string> getAssetNames(){
            T empt;
            std::vector<std::string> assetList = getAssetNamesInternal(empt);
            return assetList;
        }
        /**
            Specialized template function gen.
            *Use AssetMannager::make<T> to create an texture
            @param forced force to generate again that texture. Overhiding the old one. Old references may persist
            @param spr Not used, only to diferentiate
            @param str file name
            @param ...args variadic arguments for the creator funcion
            @return the generated TexturePtr
        */
        template<typename ...Args> TexturePtr gen(bool forced,Sprite spr,std::string str,Args ...args){
            return makeTexture(forced,str,args...);
        }
        /**
            *Specialized template function gen.
            *Use AssetMannager::make<T> to create an texture
            @param forced force to generate again that sound buffer. Overhiding the old one. Old references may persist
            @param snd Not used, only to diferentiate
            @param str file name
            @param ...args variadic arguments for the creator funcion
            @return the generated SoundPtr
        */
       template<typename ...Args> SoundPtr gen(bool forced,Sound snd,std::string str,Args ...args){
            return makeSound(forced,str,args...);
        }
        /**
            * Reload an given asset. This will affect all other instances using its reference.
            * If calling the singe creator of Sprite or Sound, it will be allocd in the global asset manager
            * Its better use local assetmanagers
            @code
                // Sprite("ball.png");
                Assets.reload<Sprite>("ball.png");
                Assets.reload<Sprite>("dogurai:animated.png",3,1.0f);
            @endcode

            @param ...args arguments used in creating an new Sprite or Sound. Check its creators
        */
        template<class T,typename ...Args> T reload(Args ...args){
            auto t = gen(true,T(),args...);
            if (t.get()){
                T sp(t,args...);
                return sp;
            }
            return T();
        }
        /**
            * Load an given asset. If the asset is already loaded you will receive an reference.
            * If calling the singe creator of Sprite or Sound, it will be allocd in the global asset manager
            * Its better use local assetmanagers
            @code
                // Sprite("ball.png");
                Assets.make<Sprite>("ball.png");
                Assets.make<Sprite>("dogurai:animated.png",3,1.0f);
            @endcode

            @param ...args arguments used in creating an new Sprite or Sound. Check its creators
        */
        template<class T,typename ...Args> T make(Args ...args){
            auto t = gen(false,T(),args...);
            if (t.get()){
                T sp(t,args...);
                return sp;
            }
            return T();
        }

        template<class T,typename ...Args> std::shared_ptr<T> make_shared(Args ...args){
            auto t = gen(false,T(),args...);
            if (t.get()){
                std::shared_ptr<T> sp(new T(t,args...));
                return sp;
            }
            return std::shared_ptr<T>(new T);
        }

        TexturePtr makeTexture(bool forced,std::string str,int fcount=1,float ftime=0,int rep=1,bool hasAliasing=false);
        TexturePtr makeTexture(bool forced,SDL_RWops* rw,std::string str,bool hasAntiAliasign=false);
        TexturePtr makeTexture(bool forced,std::string fileName,ColorReplacer &r,bool HasAliasing=false);


        SoundPtr makeSound(bool forced,std::string fileName);
        SoundPtr makeSound(bool forced,SDL_RWops* rw,std::string str);

        std::vector<std::string> getAssetNamesInternal(Sprite){
            std::vector<std::string> stuff;
            for (auto &it : spriteMap){
                stuff.emplace_back(it.first);
            }
            return stuff;
        }

        std::vector<std::string> getAssetNamesInternal(Sound){
            std::vector<std::string> stuff;
            for (auto &it : soundMap){
                stuff.emplace_back(it.first);
            }
            return stuff;
        }

    private:






        static uint32_t ManagerId;
        uint32_t id;

        std::unordered_map<std::string, TexturePtr> spriteMap;
        std::unordered_map<std::string, SoundPtr> soundMap;
        //std::unordered_map<std::string, MusicPtr> musicMap;


        static std::map<uint32_t,AssetMannager*> AssetMannagerMap;
		bool setOutput;
};





class GlobalAssetManager: public AssetMannager{
    public:
        static GlobalAssetManager &GetInstance();
};

class UIAssetManager: public AssetMannager{
    public:
        static UIAssetManager &GetInstance();
};

