#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER


#ifndef SPRITEHA
#define SPRITEHA

#include "../framework/geometry.hpp"
#include "../framework/chainptr.hpp"

#include <unordered_map>
#include <map>
#include <string>

#include <memory>
typedef chain_ptr<SDL_Texture> TexturePtr;

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
    private:
        std::map<uint32_t,bool> canReplace;
        std::map<uint32_t,uint32_t> Replace;
};

class AssetMannager;



/**
 * @brief Sprite class plus animation
 *
 * This class hold animations and sprites
 * Some sprites textures may be shared. Check Sprite::assetTable
 */


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
        Sprite(TexturePtr texture,std::string name,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);
        /**
            *Create an sprite from an path to an file. The file should be SDL2_Image supported
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(char *file,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);
        /**
            *Create an sprite from an path to an file. The file should be SDL2_Image supported
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(const char *file,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);
        /**
            *Create an sprite from an path to an file. The file should be SDL2_Image supported
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param r An replacer filter
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(const char *file,ColorReplacer &r,bool replaceOnAssets=true,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);


        Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);
        /**
            *Create an sprite from a rwops. Also delete the RWops
            *You also NEED to set an alias to use as hash.
            *You can set the frame count and frame time to an animation
            *When you animate an sprite, remember to call Sprite::Update on your GameActivity
            @param file The path to an file
            @param fcount Frame count. <b>THE FRAME MOVES ON X ONLY<b>
            @param ftime The time between the frames
        */
        Sprite(SDL_RWops* rw,std::string name,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false);

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

        static SDL_Texture* Preload(const char *file,bool adjustDir=true,bool HasAliasing=false);
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
        static SDL_Texture* Preload(SDL_RWops* rw,std::string name,bool HasAliasing=false);
        /**
            *Works like Sprite::Preload(char *file,ColorReplacer &r,bool replaceOnAssets=true,int fcount=1,float ftime = 1,int repeat=1,bool hasAliasing=false)
            *You have to pass an RWops and set an alias to work on Sprite::assetTable
            @param fileName file name. Accept an alias like assets:file.png
            @param r an color replace filer.
            @param HasAliasing mark as true to load the sprite using antialiasing.
        */
        static SDL_Texture *Preload(std::string fileName,ColorReplacer &r,bool HasAliasing=false);
        /**
            *This dont destroy the texture!!!
        */
        Sprite* GetMe(){
            return this;
        }
        ~Sprite();
        /**
            *Can be used when you create an sprite with empty constructor.
            @param rw The rwops
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
       bool Openrw(SDL_RWops* rw,std::string name,bool HasAliasing=false){
            return Open(rw,name,HasAliasing);
       };
       bool Open(SDL_RWops* rw,std::string name,bool HasAliasing=false);
        /**
            *Can be used when you create an sprite with empty constructor.
            @param file The path. Accept resource tweak.
            @param name Sprite alians
            @param reopen Default is false. When you call this, a new texture will be created and dont be added to the Sprite::assetTable
            @return An texture
        */
        bool Openf(std::string file,bool HasAliasing=false){
            return Open(file.c_str(),HasAliasing);
        }
        bool Open(const char *file,bool HasAliasing=false);
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

        void Renderxy(int x,int y,double angle=0);
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
            *The render function will render on the game at the position PointInt(x,y).
            *When screne is streched or full screen, this function will take care of anything
            *Dont worry about scaling or offsets from anything on this function.
            *Unless you set your only offset.
            @param pos is a PointInt (x,y)
            @param angle When you need rotate the sprite
        */
        void Renderpoint(PointInt pos,double angle=0){
            Render(pos,angle);
        }

        /**
            *This should be call at each game loop only if you are animating something
            @param dt The time between the frames
        */
        void Update(float dt);
        /**
            *Just reset the animation to the fame 0. As you can use an grid you can
            *also reset the y axis by setting the parameter as true.
            *
            *This will reset only the internal counter of the repeat. See Sprite::IsAnimationOver
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
            *Get the Frame width
            *
            *<b>Is affected by scaling</b>
            @return The size
        */
        int GetFrameWidth();
        /**
            *Get the Frame height
            *
            *<b>Is affected by scaling</b>
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
        /**
            *Return the frame count
            @return Return the frame count
        */
        int GetFrameCount(){
            return frameCount;
        };

        Point GetCurrentFrame(){
            return Point(currentFrame);
        };
        /**
            *Change the frame time
            *This also will <b>reset the current timer</b> of the current frame.
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
            return textureShred.get() != nullptr ;
        };
        /**
            *Get the texture. Note that texture can be an shared one
            @return The SDL_Texture pointer;
        */
        SDL_Texture* GetSDLTexture(){
            return (textureShred.get());
        };
        /**
            *Used to change the center of rotation
            @param p Sprite position (inside the image)
        */
        void SetCenter(Point p){
            hasCenter=true;
            center.x = (int)p.x;
            center.y = (int)p.y;
        };

        Point GetCenter(){
            return Point(center);
        }

        /**
            *Set the amount of times the animation will repeat until Sprite::IsAnimationOver could return true
            *Say you set repeat to 2, then every time it finishes the animation cycle an counter will be incremented
            *once it reaches the same value as you set on Sprite::SetRepeatTimes it will be able to return true.
            *This is not a trigger.
            @param t repeat times
        */
        void SetRepeatTimes(int t){
            repeat = t;
        }
        int GetRepeatTimes(){
            return repeat;
        }
        /**
            *Check if the animation has finished
            @param if the animation is over
        */
        bool IsAnimationOver(){
            return over >= repeat;
        };
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */
        void SetScaleX(float scale=1){
            scaleX=scale;
        };
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */

        void SetCenteredScale(bool set,Point sprSize=Point(0,0)){
            scaleCentered = set;
            if (sprSize.x == 0 || sprSize.y == 0){
                widSize = Point(clipRect.w,clipRect.h);
            }else{
                widSize = sprSize;
            }
        }

        void SetScaleY(float scale=1){
            scaleY=scale;
        };
        /**
            *Scale the texture.
            @param scale The scale in x and y codinates. Default argument is 1,1
        */
        void SetScale(Point scale = Point(1,1)){
            scaleX=scale.x;
            scaleY=scale.y;
        };

        Point GetScale(){
            return Point(scaleX,scaleY);
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
            SDL_SetTextureColorMod((textureShred.get()),OUTR,OUTB,OUTG);
        };
        /**
            *Changed the sprite alpha
            *This changes the texture, so its <b>SHARED</b>
            *When set something here, if you use the same sprite in another place, its good to restore
            @param alpha [0-255] The default is 255 of all sprites;
        */
        void SetAlpha(uint8_t alpha){
            m_alpha = alpha;
            SDL_SetTextureAlphaMod((textureShred.get()),alpha);
        };

        uint8_t GetAlpha(){
            return m_alpha;
        };
        /**
            *Duplicate the texture
            *<b>Not tested yet with all features</b>
        */
        SDL_Texture* CopyTexture();
        /**
            *Set a grid for animation frames
            @param gx The grid size in x axis
            @param gy The grid size in y axis
        */
        void SetGrid(int gx,int gy);

        PointInt GetGrid(){
            return grid;
        };
        /**
            Flip the current sprite locally
            @param flipState
        */
        void SetFlip(SDL_RendererFlip flipState){
            sprFlip = flipState;
        }

        SDL_RendererFlip GetFlip(){
            return sprFlip;
        }

        /**
            Get the current frame in the grid
            @return the frame on x,y position
        */
        PointInt GetFrame(){ return currentFrame; };
        /**
            Used to load texture settings from the given pointer
            @param ptr The pointer
        */
        void Query(TexturePtr ptr);
        /**
            Placeholder function. Not used
        */
        void Kill();
    private:
        bool aliasing;
        TexturePtr textureShred;
        friend class AssetMannager;
        SDL_RendererFlip sprFlip;
        std::string fname;
        uint8_t OUTR,OUTB,OUTG;
        uint8_t m_alpha;
        float scaleX,scaleY,timeElapsed,frameTime;
        int over;
        int repeat;
        int frameCount;
        PointInt currentFrame;
        PointInt grid;
        SDL_Rect dimensions;
        SDL_Rect clipRect;
        SDL_Point center;
        Point widSize;
        bool hasCenter,scaleCentered;
};



class Animation{
    public:

        Animation():Loops(0),sprX(0),sprY(0),sprW(0),sprH(0),MaxFrames(01),SprDelay(1.0f),SprMaxDelay(1.0f),CanRepeat(true),
        pause(false),finishedFrame(false),finishedSingleFrame(false),isFormated(false){};
        Animation(float w,float h):Animation(){
           sprW = w;
           sprH = h;
        };
        void Update(float dt){
            if (pause){
                return;
            }
            finishedFrame = finishedSingleFrame = false;
            SprDelay -= dt;
            if (SprDelay <= 0){
                finishedSingleFrame = true;
                sprX++;
                SprDelay = SprMaxDelay;
                if (sprX >= MaxFrames){
                    if (CanRepeat){
                        Loops++;
                        sprX = 0;
                    }else{
                        sprX--;
                    }
                    finishedFrame = true;
                }
            }
        }
        void Pause(bool p){
            pause = p;
        }
        bool IsAnimationEnd(){
            return finishedFrame;
        }
        bool IsFrameEnd(){
            return finishedSingleFrame;
        }
        void SetAnimation(int y,int maxFrames,float timer=-1){
            if (timer >= 0){
                SetAnimationTime(timer);
            }
            sprY = y;
            MaxFrames = maxFrames;
            ResetAnimation();
        }
        void ResetAnimation(){
            sprX = 0;
            SprDelay = SprMaxDelay;
            finishedFrame = false;
            finishedSingleFrame = false;
            Loops = 0;
        }
        void SetAnimationTime(float time){
            SprDelay = SprMaxDelay = time;
        }

        void FormatSprite(Sprite& sp,int dir){
            if (!sp.IsLoaded()){
                return;
            }
            if (dir == 0){
                sp.SetFlip(SDL_FLIP_NONE);
            }else if (dir == 1){
                sp.SetFlip(SDL_FLIP_HORIZONTAL);
            }else if (dir == 2){
                sp.SetFlip(SDL_FLIP_VERTICAL);
            }
            sp.SetClip(sprX * sprW, sprY * sprH,sprW,sprH);
            isFormated = true;
        }
        void Render(float x,float y,Sprite& sp,int dir,float angle=0){
            if (!isFormated){
                FormatSprite(sp,dir);
            }
            sp.Render(PointInt(x,y),angle);
            isFormated = false;
        }
        uint32_t Loops;
        uint32_t sprX;
        uint32_t sprY;
        uint32_t sprW;
        uint32_t sprH;
        uint32_t MaxFrames;
        float SprDelay;
        float SprMaxDelay;
        bool CanRepeat;
    private:
        bool pause;
        bool finishedFrame;
        bool finishedSingleFrame;
        bool isFormated;
};

#endif
