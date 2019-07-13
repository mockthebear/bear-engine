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
#include "animation/scriptloader.hpp"

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

AnimatedSprite Sprite::Animate(int fcount,float ftime,int repeat){
    AnimatedSprite as(std::move(*this));
    return as;
}


int AnimatedSprite::GetFrameHeight(){
    return sprH*m_renderData.GetScale().y;
}

int AnimatedSprite::GetFrameWidth(){
    return sprH*m_renderData.GetScale().y;
}

void AnimatedSprite::Update(float dt){
    if (m_scriptedAnimation.get()){
        m_scriptedAnimation->Update(dt);

        m_scriptedAnimation->UpdateSprite(*this);

        return;
    }
    Animation::Update(dt);
    FormatSprite(*this);
}

void AnimatedSprite::AddCallback(std::string labelName,std::function<bool()> &&cb){
    if (m_scriptedAnimation.get()){
        m_scriptedAnimation->AddCallback(labelName,cb);
        return;
    }
    return;
}


bool AnimatedSprite::RunAnimationSegment(std::string&& seg){
    if (m_scriptedAnimation.get()){
        m_scriptedAnimation->Run(seg);
        return true;
    }
    return false;
}

bool AnimatedSprite::LoadAnimationScript(std::string&& filepath){
    m_scriptedAnimation = std::make_shared<AnimationScript>();
    try {
        ScriptLoader::LoadScript(filepath, *m_scriptedAnimation.get());
    } catch(BearException &e){
        e.Show();
        m_scriptedAnimation.reset();
        return false;
    }
    return true;
}

void AnimatedSprite::Render(PointInt pos){
    if (!m_scriptedAnimation.get()){
        if (!isFormated){
            FormatSprite(*this);
        }
    }
    Sprite::Render(pos);
    isFormated = false;
}

AnimatedSprite::AnimatedSprite(Sprite&& sp):Sprite(sp),Animation(){
    InnerCopy(sp);
}

AnimatedSprite::AnimatedSprite(Sprite& sp):Sprite(sp),Animation(){
    InnerCopy(sp);
}


void AnimatedSprite::InnerCopy(Sprite& sp){
    m_vertexes = sp.m_vertexes;
    m_texture.reset(sp.m_texture.get());
    QueryFrames();
}




void AnimatedSprite::QueryFrames(){
    BearTexture *texturee = m_texture.get();
    if (texturee != NULL){
        size.x = sprW = texturee->size_w;
        size.y = sprH = texturee->size_h;

        TextureSize.x = texturee->texture_w;
        TextureSize.y = texturee->texture_h;
        SetClip(0,0,texturee->texture_w,texturee->texture_h);
    }
    SetGridSize(size.x / MaxFrames, size.y);
}
