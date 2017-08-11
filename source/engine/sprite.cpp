#include "sprite.hpp"
#include "gamebase.hpp"
#include <math.h>
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "renderhelp.hpp"
#include "camera.hpp"
#include "smarttexture.hpp"
#include "../framework/resourcemanager.hpp"

#include "../image/stb_image.h"


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
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    scaleX = scaleY = 1;
    currentFrame = PointInt(0,0);
    frameCount = 1;
    repeat = 1;
    hasCenter = false;
    aliasing = false;
    m_alpha = 255;
    fname = "";
    over = 0;
    OUTR = OUTB = OUTG = 255;
    timeElapsed = 0;
    frameTime = 0;
    sprFlip = SDL_FLIP_NONE;

}

Sprite::Sprite(TexturePtr texture_,std::string name,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    textureShred = texture_;
     frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    Uint32 format;
    int acess;
    aliasing = hasAliasing;
    if (textureShred.get()){
        if (SDL_QueryTexture(textureShred.get(), &format,&acess,&dimensions.w,&dimensions.h) < 0){

        }
        SetClip(0,0,dimensions.w,dimensions.h);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    aliasing = hasAliasing;
    textureShred = texture_;
    Query(textureShred);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(const char *file,ColorReplacer &r,bool replacer,int fcount,float ftime,int rep,bool hasAliasing):Sprite(){
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = file;
    aliasing = hasAliasing;
    scaleX = scaleY = 1;
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,file,r,hasAliasing);
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);

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
    Open(file,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

void Sprite::Kill(){
    //textureShred.destroy();
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
        SDL_RWops *rw = ResourceManager::GetInstance().GetFile(stdnamee); //Safe
        SDL_Texture* returnTexture = Sprite::Preload(rw,stdnamee,HasAliasing);
        SDL_RWclose(rw);
        return returnTexture;
    }
    if (adjustDir){
        stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    }
    unsigned char* imageData = nullptr;
    SDL_RWops* rw = SDL_RWFromFile(file, "rb");
    if (!rw){
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",stdnamee.c_str(),SDL_GetError()));
        return nullptr;
    }
    uint64_t rsize;
    int sizeX,sizeY,comp;
    char *res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    SDL_RWclose(rw);
    if (imageData){
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        #else
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        #endif
        if (surface){
            /*
                Pixel replacing
            */
            int pixelCount = (sizeX*sizeY);
            for( int i = 0; i < pixelCount; ++i ){
                ((Uint32*)surface->pixels)[i] =((Uint32*)imageData)[i];
            }
            /*
                Generate texture
            */
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
            SDL_Texture *ret = SDL_CreateTextureFromSurface(Game::GetInstance()->GetRenderer(),surface);// = finalSmart->GetTexture();
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

            /*
                Cleanup and return
            */
            stbi_image_free(imageData);
            SDL_FreeSurface(surface);
            return ret;
        }else{
            stbi_image_free(imageData);
            Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",file,SDL_GetError()));
        }
    }
    return nullptr;
}

SDL_Texture* Sprite::Preload(SDL_RWops* rw,std::string name,bool HasAliasing){
    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    uint64_t rsize;
    if (rw == nullptr){
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload rw sprite [%s]",name.c_str()));
        return nullptr;
    }
    char *res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    if (imageData){
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        #else
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        #endif
        if (surface){
            /*
                Pixel replacing
            */
            int pixelCount = (sizeX*sizeY);
            for( int i = 0; i < pixelCount; ++i ){
                ((Uint32*)surface->pixels)[i] = (((Uint32*)imageData)[i]);
            }
            /*
                Generate texture
            */
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
            SDL_Texture *ret = SDL_CreateTextureFromSurface(Game::GetInstance()->GetRenderer(),surface);// = finalSmart->GetTexture();
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

            /*
                Cleanup and return
            */
            stbi_image_free(imageData);
            SDL_FreeSurface(surface);

            return ret;
        }else{
            stbi_image_free(imageData);
            Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",name.c_str(),SDL_GetError()));
        }
    }


    if (HasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

    return nullptr;
}
SDL_Texture* Sprite::Preload(std::string fileName,ColorReplacer &r,bool HasAliasing){
    if (fileName == ""){
        return nullptr;
    }
    fileName = DirManager::AdjustAssetsPath(fileName);
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    if (fileName.find(":")!=std::string::npos){
        /**
            Loading from rwops
        */
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(fileName); //safe
        if (!rw){
            bear::out << "Error loading ["<<fileName<<"]: " << SDL_GetError() << "\n";
            return nullptr;
        }
        uint64_t rsize;
        char *res = ResourceManager::GetFileBuffer(rw,rsize);
        imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
        ResourceManager::ClearFileBuffer(res);
        SDL_RWclose(rw);
    }else{
        imageData = stbi_load(fileName.c_str(),&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    }
    if (imageData){
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        #else
        SDL_Surface* surface = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        #endif
        if (surface){
            /*
                Pixel replacing
            */
            int pixelCount = (sizeX*sizeY);
            for( int i = 0; i < pixelCount; ++i ){
                ((Uint32*)surface->pixels)[i] = r.Get(((Uint32*)imageData)[i]);
            }
            /*
                Generate texture
            */
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
            SDL_Texture *ret = SDL_CreateTextureFromSurface(Game::GetInstance()->GetRenderer(),surface);// = finalSmart->GetTexture();
            if (HasAliasing)
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

            /*
                Cleanup and return
            */
            stbi_image_free(imageData);
            SDL_FreeSurface(surface);
            return ret;
        }else{
            stbi_image_free(imageData);
            Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",fileName.c_str(),SDL_GetError()));
        }
    }
    return nullptr;
}

void Sprite::Query(TexturePtr ptr){
    SDL_Texture *texturee = ptr.get();
    if (texturee != NULL){
        Uint32 format;
        int acess;
        SDL_QueryTexture((texturee), &format,&acess,&dimensions.w,&dimensions.h);
        SetClip(0,0,dimensions.w,dimensions.h);
    }
}

bool Sprite::Open(const char *file,bool HasAliasing){
    scaleX = scaleY = 1;
    std::string stdnamee(file);
    if (stdnamee.find(":")!=std::string::npos){
        SDL_RWops* file = ResourceManager::GetInstance().GetFile(stdnamee); //safe
        bool ret = Open(file,stdnamee,HasAliasing);
        SDL_RWclose(file);
        return ret;
    }
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,stdnamee,HasAliasing);
    if (textureShred.get()){
        Query(textureShred);
        return true;
    }
    return false;
}

bool Sprite::Open(SDL_RWops* file,std::string name,bool HasAliasing){
    scaleX = scaleY = 1;
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,file,name,HasAliasing);
    if (textureShred.get()){
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
        return false;
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
    SDL_RenderCopyEx(BearEngine->GetRenderer(),textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip);

}
void Sprite::Renderxy(int x,int y,double angle){
    Render(x-Camera::pos.x,y-Camera::pos.y,angle);
}
void Sprite::Render(int x,int y,double angle){
    SDL_Rect dimensions2;

    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    dimensions2.x = x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
    dimensions2.y = y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
    dimensions2.h = clipRect.h*scaleRatioH*scaleY;
    dimensions2.w = clipRect.w*scaleRatioW*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
}


void Sprite::RawRender(int x,int y,double angle){
    SDL_Rect dimensions2;
    dimensions2.x = x;
    dimensions2.y = y;
    dimensions2.h = clipRect.h*scaleY;
    dimensions2.w = clipRect.w*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
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
