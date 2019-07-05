#include "sprite.hpp"
#include "gamebase.hpp"
#include <math.h>
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "renderhelp.hpp"
#include "camera.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/debughelper.hpp"
#include "../image/stb_image.h"

TextureLoadMethod TextureLoadMethod::DefaultLoadingMethod = TEXTURE_NEAREST;

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


Sprite::Sprite(TexturePtr texture_,std::string name,std::string alias,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    m_texture = texture_;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
}

Sprite::Sprite(TexturePtr texture_,std::string name,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    m_texture = texture_;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
}

Sprite::Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode= TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    aliasing = hasAliasing;
    m_texture = texture_;
    Query(m_texture);

}


Sprite::Sprite(SDL_RWops* file,std::string name,TextureLoadMethod hasAliasingg):Sprite(){
    if (hasAliasingg.mode == TEXTURE_DEFAULT){
        hasAliasingg.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    Open(file,name,hasAliasingg);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    Open(file,hasAliasing);
    SetAlpha(255);
}


void Sprite::Update(float dt){}


BearTexture *Sprite::Preload(const char *file,bool adjustDir,TextureLoadMethod HasAliasing){
    if (HasAliasing.mode == TEXTURE_DEFAULT){
        HasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    char sprn[1024];
    strcpy(sprn,file);
    std::string stdnamee(sprn);

    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        SDL_RWops *rw = ResourceManager::GetInstance().GetFile(stdnamee); //Safe
        if (!rw){
            Console::GetInstance().AddTextInfo(utils::format("[RW] 1 Cannot preload sprite [%s] because: %s",aux.c_str(),SDL_GetError()));
            return Painter::UnloadedTexture;
        }
        BearTexture *returnTexture = Sprite::Preload(rw,stdnamee,HasAliasing);
        SDL_RWclose(rw);
        return returnTexture;
    }
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);

    unsigned char* imageData = nullptr;
    SDL_RWops* rw = SDL_RWFromFile(stdnamee.c_str(), "rb");
    if (!rw){
        Console::GetInstance().AddTextInfo(utils::format("[SDLOpen] 2 Cannot preload sprite [%s] because: %s",stdnamee.c_str(),SDL_GetError()));
        return Painter::UnloadedTexture;
    }
    uint64_t rsize;
    int sizeX,sizeY,comp;
    char* res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    SDL_RWclose(rw);
    if (imageData){
        BearTexture *ret = Painter::MakeTexture(PointInt(sizeX,sizeY),GL_RGBA,imageData,HasAliasing);
        stbi_image_free(imageData);
        return ret;
    }
    return Painter::UnloadedTexture;
}

BearTexture * Sprite::Preload(SDL_RWops* rw,std::string name,TextureLoadMethod hasAliasing){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    uint64_t rsize;
    if (rw == nullptr){
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload rw sprite [%s]",name.c_str()));
        return Painter::UnloadedTexture;
    }
    char *res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    if (imageData){
        BearTexture *ret = Painter::MakeTexture(PointInt(sizeX,sizeY),GL_RGBA,imageData,hasAliasing);
        stbi_image_free(imageData);
        return ret;
    }


    return Painter::UnloadedTexture;
}
BearTexture * Sprite::Preload(std::string fileName,ColorReplacer &r,TextureLoadMethod hasAliasing){

    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    if (fileName == ""){
        return Painter::UnloadedTexture;
    }
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    std::string aux = fileName;
    if (ResourceManager::IsValidResource(aux)){
        /**
            Loading from rwops
        */
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(fileName); //safe
        if (!rw){
            bear::out << "Error loading ["<<fileName<<"]: " << SDL_GetError() << "\n";
            return Painter::UnloadedTexture;
        }
        uint64_t rsize;
        char *res = ResourceManager::GetFileBuffer(rw,rsize);
        imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
        ResourceManager::ClearFileBuffer(res);
        SDL_RWclose(rw);
    }else{
        GameFile tempFile;
        if (tempFile.Open(fileName,true)){
            tempFile.Cache();
            char *res = tempFile.GetCache_Unsafe();
            uint64_t rsize = tempFile.GetSize();
            imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
            tempFile.Close();
        }
    }
    if (imageData){

        /*
            Pixel replacing
        */

        int pixelCount = (sizeX*sizeY);
        for( int i = 0; i < pixelCount; ++i ){
            ((Uint32*)imageData)[i] = r.Get(((Uint32*)imageData)[i]);
        }

        BearTexture *ret = Painter::MakeTexture(PointInt(sizeX,sizeY),GL_RGBA,imageData,hasAliasing);
        stbi_image_free(imageData);
        return ret;
    }
    return Painter::UnloadedTexture;
}

void Sprite::Query(TexturePtr ptr){
    BearTexture *texturee = ptr.get();
    if (texturee != NULL){
        size.x = texturee->size_w;
        size.y = texturee->size_h;

        TextureSize.x = texturee->texture_w;
        TextureSize.y = texturee->texture_h;
        SetClip(0,0,texturee->texture_w,texturee->texture_h);
    }
}

bool Sprite::Open(std::string filepath,TextureLoadMethod hasAliasing){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    std::string stdnamee(filepath);
    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        SDL_RWops* file = ResourceManager::GetInstance().GetFile(stdnamee); //safe
        bool ret = Open(file,stdnamee,hasAliasing);
        SDL_RWclose(file);
        return ret;
    }
    m_texture = GlobalAssetManager::GetInstance().makeTexture(false,stdnamee,hasAliasing);
    if (m_texture.get()){
        Query(m_texture);
        return true;
    }
    return false;
}

bool Sprite::Open(SDL_RWops* file,std::string name,TextureLoadMethod HasAliasing){
    m_texture = GlobalAssetManager::GetInstance().makeTexture(false,file,name,HasAliasing);
    if (m_texture.get()){
        Query(m_texture);
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
    if (m_trueVirtualSize.x != 0){
        m_renderData.SetClip(Rect(x,y,w,h),m_trueVirtualSize);
    }else{
        m_renderData.SetClip(Rect(x,y,w,h),TextureSize);
    }
}

void Sprite::SetRotation(float angle){
    m_renderData.SetAngle(angle);
}


void Sprite::Render(PointInt pos){
    if (IsLoaded()){
        m_renderData.SetPosition(pos);
        RenderAll();
    }else if(m_trueVirtualSize.x >= 0){
        m_renderData.SetPosition(pos);
        m_vertexes->AddRect(m_renderData);
    }
}
void Sprite::Renderxy(int x,int y){
    Render(PointInt(x,y));
}

void Sprite::Format(Animation& anim){
    if (IsLoaded()){
        return;
    }
    SetClip(anim.sprX * anim.sprW, anim.sprY * anim.sprH,anim.sprW,anim.sprH);
    anim.isFormated = true;
}

int Sprite::GetWidth(){
    return size.x*m_renderData.GetScale().x;
}

int Sprite::GetHeight(){
    return size.y*m_renderData.GetScale().y;
}
void Animation::Update(float dt){
    if (pause){
        return;
    }
    if (LockedFinished)
        finishedAnimation = false;
    finishedSingleFrame = false;
    SprDelay -= dt;
    if (SprDelay <= 0){
        isFormated = false;
        finishedSingleFrame = true;
        LastFrame = sprX;
        sprX++;
        SprDelay = SprMaxDelay;
        if (sprX >= MaxFrames){
            if (CanRepeat){
                Loops++;
                sprX = 0;
                RepeatTimes--;
            }else{
                sprX--;
                LockedFinished = false;
                finishedSingleFrame = false;
            }
            if (RepeatTimes <= 0)
                finishedAnimation = true;
        }
    }
}

void Animation::SetFrame(int x,int y){
    if (y != -1){
        sprY = y;
    }
    sprX = x;
    if (sprX >= MaxFrames){
        sprX = 0;
    }
    isFormated = false;
}

PointInt Animation::GetFrame(){
    return PointInt(sprX, sprY);
}


void Animation::SetFrameCount(int fc){
    MaxFrames     =   std::max(1, fc);
    finishedAnimation =   false;
};

int Animation::GetFrameCount(){
    return MaxFrames;
};

void Animation::SetFrameTime(float ft){
    SprMaxDelay = ft;
    SprDelay = 0;
};

float Animation::GetFrameTime(){
    return SprMaxDelay;
};

void Animation::SetRepeatTimes(int t){
    LastRepeatCount = RepeatTimes = t;
}

int Animation::GetRepeatTimes(){
    return RepeatTimes;
}



void Animation::SetAnimation(int y,int maxFrames,float timer){
    if (timer >= 0){
        SetAnimationTime(timer);
    }
    isFormated = false;
    sprY = y;
    MaxFrames = maxFrames;
    ResetAnimation();
}

void Animation::ResetAnimation(){
    LockedFinished = true;
    sprX = 0;
    SprDelay = SprMaxDelay;
    finishedAnimation = false;
    finishedSingleFrame = false;
    isFormated = false;
    Loops = 0;
    RepeatTimes = LastRepeatCount;
}

void Animation::SetAnimationTime(float time){
    SprDelay = SprMaxDelay = time;
}

void Animation::FormatSprite(Sprite& sp){
    sp.SetClip(sprX * sprW, sprY * sprH,sprW,sprH);
    isFormated = true;
}

void Animation::RenderL(float x,float y,Sprite sp){
    if (!isFormated){
        FormatSprite(sp);
    }
    sp.Render(PointInt(x,y));
    isFormated = false;
}

void Animation::Render(float x,float y,Sprite& sp){
    if (!isFormated){
        FormatSprite(sp);
    }
    sp.Render(PointInt(x,y));
    isFormated = false;
}


int AnimatedSprite::GetFrameHeight(){
    return sprH*m_renderData.GetScale().y;
}

int AnimatedSprite::GetFrameWidth(){
    return sprH*m_renderData.GetScale().y;
}

void AnimatedSprite::Update(float dt){
    Animation::Update(dt);
    FormatSprite(*this);
}

void AnimatedSprite::Render(PointInt pos){
    if (!isFormated){
        FormatSprite(*this);
    }
    Sprite::Render(pos);
    isFormated = false;
}

AnimatedSprite::AnimatedSprite(Sprite&& sp):Sprite(),Animation(){
    m_trueVirtualSize = sp.m_trueVirtualSize;
    aliasing = sp.aliasing;
    m_renderData = sp.m_renderData;
    size = sp.size;
    TextureSize = sp.TextureSize;
    renderCycle = sp.renderCycle;
    m_texture.reset(sp.m_texture.get());
}

AnimatedSprite Sprite::Animate(int fcount,float ftime,int repeat){
    AnimatedSprite as(std::move(*this));
    return as;
}

void AnimatedSprite::Query(TexturePtr ptr){
    BearTexture *texturee = ptr.get();
    if (texturee != NULL){
        size.x = sprW = texturee->size_w;
        size.y = sprH = texturee->size_h;

        TextureSize.x = texturee->texture_w;
        TextureSize.y = texturee->texture_h;
        SetClip(0,0,texturee->texture_w,texturee->texture_h);
    }
}
