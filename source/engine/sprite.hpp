#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include SDL_IMAGE_LIB_HEADER

#ifndef SPRITEHA
#define SPRITEHA

#include "../framework/geometry.hpp"

#include <unordered_map>
#include <map>
#include <string>

/**
 * @brief Sprite class plus animation
 *
 * This class hold animations and sprites
 * Some sprites textures may be shared. Check Sprite::assetTable
 */

class ColorReplacer{
    public:
        ColorReplacer();
        void AddReplacer(uint32_t from,uint32_t to);
        uint32_t Get(uint32_t color);
    private:
        std::map<uint32_t,bool> canReplace;
        std::map<uint32_t,uint32_t> Replace;
};

class Sprite{
    public:


        /**
            *LEL, dont waste your time on empty constructor. srsly
            *it just start with 0 on everything
        */
        Sprite();
        /**
            *This constructor its a bit special, and need a bit more attention
            *You start the sprite and pass only an SDL_Texture. There is no mapping
            *No destory, not anything, it just hold the SDL_Texture and render as an common texture
            *in the class.
            @param texture An sdl texture
        */
        Sprite(SDL_Texture* texture,std::string name);
        /**
            *Create an sprite from an path to an file. The file should be SDL2_Image supported
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(char *file,int fcount=1,float ftime = 1,int repeat=1);
        /**
            *Create an sprite from an path to an file. The file should be SDL2_Image supported
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(const char *file,int fcount=1,float ftime = 1,int repeat=1);

        Sprite(const char *file,ColorReplacer &r,bool replaceOnAssets=true,int fcount=1,float ftime = 1,int repeat=1);
        /**
            *Create an sprite from a rwops. Also delete the RWops
            *You also NEED to set an alias to use as hash.
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(SDL_RWops* rw,std::string name,int fcount=1,float ftime = 1,int repeat=1);

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

        static SDL_Texture* Preload(char *file,bool adjustDir=true);
        /**
            *Works like Sprite::Preload(char *file)
            *You have to pass an RWops and set an alias to work on Sprite::assetTable
            @param rw The rwop
            @param name An alias/name to the sprite
            @code
            Spirte::Preload(
                            ResourceManager::GetInstance().GetFile("ycoins","coin.png"),
                            "coin"
                            );
            @endcode
        */
        static SDL_Texture* Preload(SDL_RWops* rw,std::string name);

        static SDL_Texture *ColorReplace(std::string fileName,ColorReplacer &r,bool replaceOnAssets=true);
        /**
            *This dont destroy the texture!!!
        */
        ~Sprite();
        /**
            *Its an map to hold all the Textures loaded. Use Sprite::Clear to erase.
        */
        static std::unordered_map<std::string, SDL_Texture*> assetTable;
        /**
            *This function is static. You can call it any time
            *Delete all the SDL_Texture loaded in the game
            *When you do this, even the textures that are being used will be deleted
            *Use with care.
            *Good to mention that some sprites may be shared.
        */
        static void Clear();


        /**
            *Can be used when you create an sprite with empty constructor.
            @param rw The rwops
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
        SDL_Texture* Open(SDL_RWops* rw,std::string name,bool reopen=false);
        /**
            *Can be used when you create an sprite with empty constructor.
            @param file The path. Accept resource tweak.
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
        SDL_Texture* Open(char *file,bool reopen=false);
        /**
            *Set clip on the sprite. Clipping stuff.
            *When call Sprite::Render, the rendered thing will be only the clipped area.
            @param x The x position on the sprite
            @param y The y position on the sprite
            @param w Width
            @param h Height
        */
        void SetClip (int x,int y,int w,int h);
        /**
            *The render function will render on the game at the position x,y.
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param x The x
            @param x The y
            @param angle When you need rotate the sprite
        */
        void Render (int x,int y,double angle=0);
        /**
            *Work as the same of Render, but without any changing by the screen scale
            *Only the transformations of this sprite
            @param x The x position
            @param y The y position
            @param angle When you need rotate the sprite
        */
        void RawRender(int x,int y,double angle);

        /**
            *The render function will render on the game at the position x,y.
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param pos is a Point (x,y)
            @param angle When you need rotate the sprite
        */
        void Render(PointInt pos,double angle=0);

        /**
            *This should be call at each game loop only if you are animating something
            @param dt The time between the frames
        */
        void Update(float dt);
        /**
            *Just reset the animation to the fame 0. As you can use an grid you can
            *also reset the y axis by setting the parameter as true.
        */

        void ResetAnimation(bool changeYaxis=false){
            over = 0;
            currentFrame.x = 0;
            timeElapsed = 0;
            if (changeYaxis)
                currentFrame.y = 0;

        }
        /**
            *You can force an frame.
            *As the class only changes the X axis, you can set the yFrame parameter
            *by your own. When set -1 will be the default or last set yFrame.
        */
        void SetFrame(int xFrame,int yFrame=-1);
        /**
            *Get the Sprite width
            @return The size
        */
        int GetWidth();
        /**
            *Get the Sprite height
            @return The size
        */
        int GetHeight();
        /**
            *Get the Frame width
            @return The size
        */
        int GetFrameWidth();
        /**
            *Get the Frame height
            @return The size
        */
        int GetFrameHeight();

        /**
            *Change the frame amount
            @param fc The new frame count
        */
        void SetFrameCount(int fc){
            frameCount=fc;
            over = false;
        };

        int GetFrameCount(){
            return frameCount;
        };
        /**
            *Change the frame time
            @param ft The new frame time
        */
        void SetFrameTime(float ft){
            frameTime=ft;
            timeElapsed = 0;

        };
        /**
            *Get the remeaining time to the next frame
            @return the time left
        */
        float GetFrameTime(){
            return frameTime;
        };
        /**
            *Change the frame amount
            @return true if the sprite is loaded
        */
        bool IsLoaded(){
            return texture != NULL;
        };
        /**
            *Get the texture. Note that texture can be an shared one
            @return true if the texture
        */
        SDL_Texture* GetSDLTexture(){
            return texture;
        };
        /**
            *Used to change the center of rotation
            @param p Sprite position (inside the image)
        */
        void SetCenter(Point p){
            hasCenter=true;
            center.x = p.x;
            center.y = p.y;
        };
        /**
            *Check if the animation have done one cycle
            *Actually return the amount of animation cycles it have made
            @return
        */
        void SetRepeatTimes(int t){
            repeat = t;
        }
        int IsAnimationOver(){
            return over >= repeat;
        };
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */
        void SetScaleX(float scale){
            scaleX=scale;
        };
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */
        void SetScaleY(float scale){
            scaleY=scale;
        };

        void SetScale(Point scale){
            scaleX=scale.x;
            scaleY=scale.y;
        };
        /**
            *You can cut some color channels and reblend the sprite
            *This changes the texture, so its <b>SHARED</b>
            *When set something here, if you use the same sprite in another place, its good to restore
            @param Red [0-255] The default is 255 of all sprites;
            @param Blue [0-255] The default is 255 of all sprites;
            @param Green [0-255] The default is 255 of all sprites;
        */
        void ReBlend(uint8_t Red,uint8_t Blue,uint8_t Green){
            OUTR = Red;
            OUTB = Blue;
            OUTG = Green;
            SDL_SetTextureColorMod(texture,OUTR,OUTB,OUTG);
        };
        /**
            *Changed the sprite alpha
            *This changes the texture, so its <b>SHARED</b>
            *When set something here, if you use the same sprite in another place, its good to restore
            @param alpha [0-255] The default is 255 of all sprites;
        */
        void SetAlpha(int alpha){
            SDL_SetTextureAlphaMod(texture,alpha);
        };
        /**
            *Duplicate the texture
            *<b>Not tested yet with all features</b>
        */
        SDL_Texture* CopyTexture();
        /**
            *Set a grid for animation frames
        */
        void SetGrid(int gx,int gy);
        /**
            Flip the current sprite locally
        */
        void SetFlip(SDL_RendererFlip f){
            sprFlip = f;
        }
    private:
        SDL_RendererFlip sprFlip;
        std::string fname;
        uint8_t OUTR,OUTB,OUTG;
        float scaleX,scaleY,timeElapsed,frameTime;
        int over;
        int repeat;
        int frameCount;
        PointInt currentFrame;
        PointInt grid;
        SDL_Texture* texture;
        SDL_Rect dimensions;
        SDL_Rect clipRect;
        SDL_Point center;
        bool hasCenter;
};

#endif
