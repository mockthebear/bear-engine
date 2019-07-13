#include "../settings/definitions.hpp"



#ifndef SPRITEHA
#define SPRITEHA

#include "../framework/geometry.hpp"
#include "../framework/chainptr.hpp"
#include "painters/painters.hpp"
#include "rendertexture.hpp"
#include "animation/animation.hpp"
#include "animation/animationscript.hpp"


#include <string>
#include <map>
#include <memory>

#include "libheader.hpp"


/**
 * @brief Color replacing filter class to use on load
 *
 * This class just can be used to replace some colors
 */

class ColorReplacer{
    public:
        /**
            Empty constructor
        */
        ColorReplacer();
        /**
            Clear everything
        */
        void clear(){
            canReplace.clear();
            Replace.clear();
        }
        /**
            *Insert an color to be change to another one.
            *Example:
            @code
            ColorReplacer replacer;
            replacer.AddReplacer(RenderHelp::FormatRGBA(255,255,255,255),RenderHelp::FormatRGBA(0,0,0,0));
            @endcode
            *Should be good to check the reference of RenderHelp::FormatRGBA and RenderHelp::FormatARGB
        */
        void AddReplacer(uint32_t from,uint32_t to);
        /**
            *Get the given color that can correspond to the one you gave
        */
        uint32_t Get(uint32_t color);

        uint32_t Size(){
            return Replace.size();
        };
    private:
        std::map<uint32_t,bool> canReplace;
        std::map<uint32_t,uint32_t> Replace;
};

class AssetMannager;
class SpriteAtlas;
class AnimatedSprite;


/**
 * @brief Sprite class plus animation
 *
 * This class hold animations and sprites
 * Some sprites textures may be shared. Check Sprite::assetTable
 */


class Sprite: public RenderTexture{
    public:

        Sprite():RenderTexture(),m_trueVirtualSize(0.0f, 0.0f){};
        /**
            *This constructor its a bit special, and need a bit more attention
            *You start the sprite and pass only an SDL_Texture. There is no mapping
            *No destory, not anything, it just hold the SDL_Texture and render as an common texture
            *in the class.
            @param texture An sdl texture
        */
        Sprite(TexturePtr texture):Sprite(){m_texture = texture;};

        Sprite(TexturePtr texture,std::string name,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);
        Sprite(TexturePtr texture,std::string name,std::string alias,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);

        Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);

        Sprite(SDL_RWops* rw,std::string name,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);


        Sprite(std::string file,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);


        void SetVirtualData(Point virtualId){
            m_trueVirtualSize = virtualId;
        }


        /**
            *This function is static. You can call it any time
            @code
            Spirte::Preload("image.png");
            @endcode
            *Also works with
            @code
            Spirte::Preload("asset:image.png");
            @endcode
            *Check ResourceManager class
            *This function may be call during an loading or something.
            *This will load the SDL_Texture an save on Sprite::assetTable
            *Then when you create an sprite that were once loaded, it will be faster.
            *The name you pass will be the alias to the sprite
            @param file The path or the asset tweak asset:file
        */

        static BearTexture* Preload(const char *file,bool adjustDir=true,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT);
        /**
            *Works like Sprite::Preload(char *file)
            *You have to pass an RWops and set an alias to work on Sprite::assetTable
            @param rw The rwop
            @param name An alias/name to the sprite
            @code
            SDL_RWops* file = ResourceManager::GetInstance().GetFile("ycoins","coin.png"); //safe
            Spirte::Preload(file,
                            "coin"
                            );
            SDL_RWclose(file);
            @endcode
        */
        static BearTexture* Preload(SDL_RWops* rw,std::string name,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT);
        /**
            *Works like Sprite::Preload(char *file,ColorReplacer &r,bool replaceOnAssets=true,int fcount=1,float ftime = 1,int repeat=1,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT)
            *You have to pass an RWops and set an alias to work on Sprite::assetTable
            @param fileName file name. Accept an alias like assets:file.png
            @param r an color replace filer.
            @param HasAliasing mark as true to load the sprite using antialiasing.
        */
        static BearTexture *Preload(std::string fileName,ColorReplacer &r,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT);
        Sprite* GetMe(){
            return this;
        }
        /**
            *This dont destroy the texture!!!
        */
        virtual ~Sprite(){};
        /**
            *Can be used when you create an sprite with empty constructor.
            @param rw The rwops
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
       bool Openrw(SDL_RWops* rw,std::string name,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT){
            return Open(rw,name,AliasingMethod);
       };
       bool Open(SDL_RWops* rw,std::string name,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT);
        /**
            *Can be used when you create an sprite with empty constructor.
            @param file The path. Accept resource tweak.
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
        bool Openf(std::string file,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT){
            return Open(file.c_str(),AliasingMethod);
        }
        bool Open(std::string file,TextureLoadMethod AliasingMethod=TEXTURE_DEFAULT);
        /**
            *Set clip on the sprite. Clipping stuff.
            *When call Sprite::Render, the rendered thing will be only the clipped area.
            @param x The x position on the sprite
            @param y The y position on the sprite
            @param w Width
            @param h Height
        */
        void SetClip (int x,int y,int w,int h);
        void SetRotation(float angle);
        /**
            *The render function will render on the game at the position x,y.
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param x The x
            @param x The y
            @param angle When you need rotate the sprite
        */
        void Renderxy(int x,int y);

        /**
            *The render function will render on the game at the position x,y.
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param pos is a Point (x,y)
            @param angle When you need rotate the sprite
        */
        virtual void Render(PointInt pos);
        /**
            *The render function will render on the game at the position PointInt(x,y).
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param pos is a PointInt (x,y)
            @param angle When you need rotate the sprite
        */
        void Renderpoint(PointInt pos){
            Render(pos);
        }

        /**
            *This should be call at each game loop only if you are animating something
            @param dt The time between the frames
        */
        void Update(float dt);
        /**
            *Get the Sprite Width
            *
            *<b>Is affected by scaling</b>
            @return The size
            @return The size
        */
        int GetWidth();
        /**
            *Get the Sprite height
            *
            *<b>Is affected by scaling</b>
            @return The size
            @return The size
        */
        int GetHeight();
        /**
            @return true if the sprite is loaded
        */
        bool IsLoaded(){
            return HasTexture();
        };
        /**
            Used to load texture settings from the given pointer
            @param ptr The pointer
        */
        virtual void Query(TexturePtr ptr);
        void Format(Animation& a);

        AnimatedSprite Animate(int fcount=1,float ftime=5.0,int repeat=1);


    private:
        friend class AssetMannager;
        friend class SpriteAtlas;
        friend class AnimatedSprite;
        Point m_trueVirtualSize;
};


class AnimatedSprite: public Sprite, public Animation{
    public:

        virtual ~AnimatedSprite(){};

        AnimatedSprite():Sprite(),Animation(){};
        AnimatedSprite(TexturePtr texture):Sprite(texture),Animation(){
            SetAnimation(0,1,5.0);
            SetRepeatTimes(1);
            QueryFrames();
        };
        AnimatedSprite(TexturePtr texture,std::string name,int fcount=1,float ftime=5.0,int repeat=1,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT):Sprite(texture,name,hasAliasing),Animation(){
            SetAnimation(0,fcount,ftime);
            SetRepeatTimes(repeat);
            QueryFrames();
        };
        AnimatedSprite(TexturePtr texture,std::string name,std::string alias,int fcount=1,float ftime=5.0,int repeat=1,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT):Sprite(texture,name,hasAliasing),Animation(){
            SetAnimation(0,fcount,ftime);
            SetRepeatTimes(repeat);
            QueryFrames();
        };
        AnimatedSprite(TexturePtr texture,std::string name,ColorReplacer &r,int fcount=1,float ftime=5.0,int repeat=1,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT):Sprite(texture,name,hasAliasing),Animation(){
            SetAnimation(0,fcount,ftime);
            SetRepeatTimes(repeat);
            QueryFrames();
        };
        AnimatedSprite(SDL_RWops* rw,std::string name,int fcount=1,float ftime=5.0,int repeat=1,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT):Sprite(rw,name,hasAliasing),Animation(){
            SetAnimation(0,fcount,ftime);
            SetRepeatTimes(repeat);
            QueryFrames();
        };
        AnimatedSprite(std::string file,int fcount=1,float ftime=5.0,int repeat=1,TextureLoadMethod hasAliasing=TEXTURE_DEFAULT):Sprite(file,hasAliasing),Animation(){
            SetAnimation(0,fcount,ftime);
            SetRepeatTimes(repeat);
            QueryFrames();
        };
        AnimatedSprite(Sprite&& sp);
        AnimatedSprite(Sprite& sp);

        /*AnimatedSprite& operator=(Sprite const &T){
            ::AnimatedSprite(T);
            return *this;
        }*/


        bool LoadAnimationScript(std::string&& filepath);
        bool RunAnimationSegment(std::string&& seg);
        void AddCallback(std::string labelName,std::function<bool()> &&cb);

        void Update(float dt);

        int GetFrameWidth();
        int GetFrameHeight();

        void Render(PointInt pos);

        void QueryFrames();
    private:
        std::shared_ptr<AnimationScript> m_scriptedAnimation;
        void InnerCopy(Sprite& sp);
};

#endif
