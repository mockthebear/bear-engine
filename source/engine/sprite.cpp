#include "sprite.hpp"
#include "gamebase.hpp"
#include <math.h>
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "renderhelp.hpp"
#include "smarttexture.hpp"
#include "../framework/resourcemanager.hpp"



ColorReplacer::ColorReplacer(){

}
void ColorReplacer::AddReplacer(uint32_t from,uint32_t to){
    canReplace[from] = true;
    Replace[from] = to;
}

uint32_t ColorReplacer::Get(uint32_t color){
    if (canReplace[color]){
        return Replace[color];
    }
    return color;
}


Sprite::Sprite(){
    ManagerId = 0;
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    scaleX = scaleY = 1;
    currentFrame = PointInt(0,0);
    frameCount = 1;
    repeat = 1;
    hasCenter = false;
    aliasing = false;
    aliasing = false;
    fname = "";
    over = 0;
    OUTR = OUTB = OUTG = 255;
    timeElapsed = 0;
    frameTime = 0;
    sprFlip = SDL_FLIP_NONE;

}

Sprite::Sprite(TexturePtr texture_,std::string name,bool hasAliasing):Sprite(){
    textureShred = texture_;
    fname = name;
    Uint32 format;
    int acess;
    if (textureShred.get()){
        if (SDL_QueryTexture(*textureShred.get(), &format,&acess,&dimensions.w,&dimensions.h) < 0){

        }
        SetClip(0,0,dimensions.w,dimensions.h);
    }
    /*if (name != "" and !assetTable[name]){
        assetTable[name] = texture_;
    }*/


    SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    SetFrame(0);
}

Sprite::Sprite(const char *file,ColorReplacer &r,bool replacer,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    /*frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = file;
    texture = ColorReplace(std::string(file),r,replacer);
    if (texture){
        Uint32 format;
        int acess;
        SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
        scaleX = scaleY = 1;
        SetClip(0,0,dimensions.w,dimensions.h);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
        TODO
    */
}

Sprite::Sprite(char *file,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    repeat = rep;
    frameTime = ftime;
    fname = file;
    Open(file,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(SDL_RWops* file,std::string name,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    Open(file,name,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(const char *file,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){

    frameCount = fcount;
    fname = file;
    frameTime = ftime;
    repeat = rep;
    Open((char *)file,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

void Sprite::Update(float dt){
    timeElapsed = timeElapsed+dt;
    if (timeElapsed >= frameTime){
        timeElapsed = 0;
        currentFrame.x++;
        if (currentFrame.x >= frameCount){
            currentFrame.x = 0;
            over++;
        }
        SetFrame(currentFrame.x);
    }
}

void Sprite::SetGrid(int gx,int gy){
    gx = std::min(gx,GetWidth());
    grid.x = gx;
    gy = std::min(gy,GetHeight());
    grid.y = gy;
    if (gx != 0){
        SetFrameCount(GetWidth()/gx);
    }

}

void Sprite::SetFrame(int xFrame,int yFrame){
    if (yFrame != -1)
        currentFrame.y = yFrame;
    currentFrame.x = xFrame;
    if (currentFrame.x >= frameCount){
        currentFrame.x = 0;
    }
    SetClip(currentFrame.x*(grid.x), currentFrame.y * (grid.y) ,grid.x,grid.y);
}

Sprite::~Sprite(){}

SDL_Texture* Sprite::CopyTexture(){
    /*if (fname != ""){
        return NULL;
    }
    return Open((char*)fname.c_str(),);*/
    return nullptr;
}


SDL_Texture* Sprite::Preload(char *file,bool adjustDir,bool HasAliasing){
    std::string stdnamee(file);
    if (stdnamee.find(":")!=std::string::npos){
        return Sprite::Preload(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee,HasAliasing);
    }
    if (adjustDir){
        stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    }
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    SDL_Texture* texture = IMG_LoadTexture(BearEngine->GetRenderer(),stdnamee.c_str());
    if (texture != NULL){
        if (HasAliasing)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
        return texture;
    }else{
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",stdnamee.c_str(),SDL_GetError()));
    }
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
    return nullptr;
}

SDL_Texture* Sprite::Preload(SDL_RWops* rw,std::string name,bool HasAliasing){
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    SDL_Texture* texture = NULL;
    SDL_Surface *aux_surface = IMG_Load_RW(rw, 1);
    if (aux_surface){
        texture =  SDL_CreateTextureFromSurface(BearEngine->GetRenderer(),aux_surface);
        SDL_FreeSurface(aux_surface);
    }
    if (texture != NULL){
        if (HasAliasing)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
        return texture;
     }else{
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",name.c_str(),SDL_GetError()));
        #ifdef __EMSCRIPTEN__
        Console::GetInstance().AddTextInfo("Resourcefiles aren't suported by emscripten. Trying instead loading.");
        std::string newName = name;
        char *c = (char*)newName.c_str();
        for (int i=0;i<newName.size();i++){
            if (c[i] == ':'){
                c[i] = '/';
                break;
            }
        }
        Console::GetInstance().AddText(utils::format("Trying instead to load [%s]",newName.c_str()));
        return Preload((char*)newName.c_str(),true,HasAliasing);
        #endif // __EMSCRIPTEN__


    }
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

    return NULL;
}
SDL_Texture* Sprite::Preload(std::string fileName,ColorReplacer &r,bool HasAliasing){
    if (fileName == ""){
        return nullptr;
    }
    fileName = DirManager::AdjustAssetsPath(fileName);
    SDL_Surface *surface = nullptr;
    if (fileName.find(":")!=std::string::npos){
        surface = IMG_Load_RW(ResourceManager::GetInstance().GetFile(fileName),1);
    }else{
        surface = IMG_Load( fileName.c_str() );
    }
    if (surface){
        if (HasAliasing)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
        SDL_PixelFormat* ScreenPixelFormat = SDL_GetWindowSurface( Game::GetInstance()->GetWindow() )->format;
        SDL_Surface* formated = SDL_ConvertSurface( surface, ScreenPixelFormat, 0 );
        if (formated){
            SDL_Texture *auxTexture = SDL_CreateTexture( Game::GetInstance()->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formated->w, formated->h );
            SmartTexture *finalSmart = new SmartTexture(0,0, formated->w, formated->h ,false);
            if (auxTexture){
                Uint32* pixels;
                int mPitch;
                SDL_LockTexture( auxTexture, NULL, (void**)&pixels, &mPitch );
				memcpy( pixels, formated->pixels, formated->pitch * formated->h );
				SDL_UnlockTexture( auxTexture );



				SDL_LockTexture( auxTexture, NULL, (void**)&pixels, &mPitch );


				Uint32* pixels2 = finalSmart->GetPixels();
                int pixelCount = ( mPitch / 4 ) * formated->h;



                Uint8 R,G,B,A;
                std::map<uint32_t,std::tuple<bool,uint32_t>> replacer;
                for( int i = 0; i < pixelCount; ++i )
                {
                    SDL_GetRGBA(pixels[ i ],ScreenPixelFormat,&R,&G,&B,&A);
                    Uint32 theColor = RenderHelp::FormatRGBA(R,B,G,A);
                    pixels2[i] = r.Get(theColor);
                }

                SDL_UnlockTexture( auxTexture );
                finalSmart->UpdateTexture();

                SDL_FreeSurface(surface);
                SDL_FreeSurface(formated);
                SDL_DestroyTexture( auxTexture );

                SDL_Texture *ret = finalSmart->GetTexture();
                if (HasAliasing)
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
                delete finalSmart;
                return ret;
            }else{
                Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",fileName.c_str(),SDL_GetError()));
            }
        }
    }
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
    return nullptr;
}

void Sprite::Query(TexturePtr ptr){
    SDL_Texture **texturee = ptr.get();
    if (texturee != NULL and (*texturee)){
        Uint32 format;
        int acess;
        SDL_QueryTexture((*texturee), &format,&acess,&dimensions.w,&dimensions.h);
        SetClip(0,0,dimensions.w,dimensions.h);
    }
}

bool Sprite::Open(char *file,bool HasAliasing){
    scaleX = scaleY = 1;
    std::string stdnamee(file);
    if (stdnamee.find(":")!=std::string::npos){
        return Open(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee,HasAliasing);
    }
    textureShred = GlobalAssetManager::GetInstance().makeTexture(stdnamee,HasAliasing);
    if (textureShred and textureShred.get() and *textureShred.get()){
        Query(textureShred);
        return true;
    }
    return false;
}

bool Sprite::Open(SDL_RWops* file,std::string name,bool HasAliasing){
    scaleX = scaleY = 1;
    textureShred = GlobalAssetManager::GetInstance().makeTexture(file,name,HasAliasing);
    if (textureShred and textureShred.get() and *textureShred.get()){
        Query(textureShred);
        return true;
    }else{
        #ifdef __EMSCRIPTEN__
        std::string newName = name;
        char *c = (char*)newName.c_str();
        for (int i=0;i<newName.size();i++){
            if (c[i] == ':'){
                c[i] = '/';
                break;
            }
        }
        Console::GetInstance().AddText(utils::format("Loading of the sprite [%s] cannot be loaded using resource tweaks at emscripten",name.c_str()));
        Console::GetInstance().AddText(utils::format("Trying instead to load [%s]",newName.c_str()));
        return Open((char*)newName.c_str());
        #else
        Console::GetInstance().AddText(utils::format("Cannot load rwop sprite FAAAAIL [%s]",name.c_str()));
        return nullptr;
        #endif // __EMSCRIPTEN__
    }
}

void Sprite::SetClip(int x, int y,int w,int h){
    clipRect.x = x;
    clipRect.y = y;
    clipRect.h = h;
    clipRect.w = w;

}


void Sprite::Render(PointInt pos,double angle){
    SDL_Rect dimensions2;


    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    dimensions2.x = pos.x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
    dimensions2.y = pos.y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
    dimensions2.h = clipRect.h*scaleRatioH*scaleY;
    dimensions2.w = clipRect.w*scaleRatioW*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),*textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip);

}
void Sprite::Render(int x,int y,double angle){
    SDL_Rect dimensions2;

    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    dimensions2.x = x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
    dimensions2.y = y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
    dimensions2.h = clipRect.h*scaleRatioH*scaleY;
    dimensions2.w = clipRect.w*scaleRatioW*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),*textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
}


void Sprite::RawRender(int x,int y,double angle){
    SDL_Rect dimensions2;
    dimensions2.x = x;
    dimensions2.y = y;
    dimensions2.h = clipRect.h*scaleY;
    dimensions2.w = clipRect.w*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),*textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
}

int Sprite::GetWidth(){
    return dimensions.w*scaleX;
}

int Sprite::GetHeight(){
    return dimensions.h*scaleY;
}
int Sprite::GetFrameHeight(){
    return dimensions.h*scaleY/frameCount;
}

int Sprite::GetFrameWidth(){
    return dimensions.w*scaleX/frameCount;
}
