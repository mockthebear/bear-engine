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
    fname = alias;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}


Sprite::Sprite(TexturePtr texture_,std::string name,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    m_texture = texture_;
    fname = name;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,std::string alias,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    m_texture = texture_;
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = alias;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    m_texture = texture_;
     frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    aliasing = hasAliasing;
    if (m_texture.get()){
        Query(m_texture);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode= TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    aliasing = hasAliasing;
    m_texture = texture_;
    Query(m_texture);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,ColorReplacer &r,bool replacer,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = file;
    aliasing = hasAliasing;
    m_texture = GlobalAssetManager::GetInstance().makeTexture(false,file,r,hasAliasing);
    if (m_texture.get()){
        Query(m_texture);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);

}


Sprite::Sprite(SDL_RWops* file,std::string name,int fcount,float ftime,int rep,TextureLoadMethod hasAliasingg):Sprite(){
    if (hasAliasingg.mode == TEXTURE_DEFAULT){
        hasAliasingg.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    Open(file,name,hasAliasingg);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    fname = file;
    Open(file,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    fname = file;
    frameTime = ftime;
    repeat = rep;
    Open(file,hasAliasing);
    frameCount = std::max(1,frameCount);
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
    if (IsAnimationOver() && m_lf > 0){
        currentFrame.x = m_lf;
        SetFrame(currentFrame.x);
        return;
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
            Console::GetInstance().AddTextInfo(utils::format("[Standard]Cannot preload sprite [%s] because: %s",aux.c_str(),SDL_GetError()));
        return nullptr;
        }
        BearTexture *returnTexture = Sprite::Preload(rw,stdnamee,HasAliasing);
        SDL_RWclose(rw);
        return returnTexture;
    }
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);

    unsigned char* imageData = nullptr;
    SDL_RWops* rw = SDL_RWFromFile(stdnamee.c_str(), "rb");
    if (!rw){
        Console::GetInstance().AddTextInfo(utils::format("[Standard]Cannot preload sprite [%s] because: %s",stdnamee.c_str(),SDL_GetError()));
        return nullptr;
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
    return nullptr;
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
        return nullptr;
    }
    char *res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    if (imageData){
        BearTexture *ret = Painter::MakeTexture(PointInt(sizeX,sizeY),GL_RGBA,imageData,hasAliasing);
        stbi_image_free(imageData);
        return ret;
    }


    return nullptr;
}
BearTexture * Sprite::Preload(std::string fileName,ColorReplacer &r,TextureLoadMethod hasAliasing){

    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    if (fileName == ""){
        return nullptr;
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
            return nullptr;
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
    return nullptr;
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
    m_renderData->SetClip(Rect(x,y,w,h),TextureSize);
}

void Sprite::Render(PointInt pos,double angle){
    if (IsLoaded()){
        m_renderData->SetPosition(pos);
        m_renderData->SetAngle(angle);
        RenderAll();
    }
}
void Sprite::Renderxy(int x,int y,double angle){
    Render(x-Camera::pos.x,y-Camera::pos.y,angle);
}
void Sprite::Render(int x,int y,double angle){
    Render(PointInt(x,y),angle);
}


void  Sprite::Format(Animation anim,int dir){
    if (IsLoaded()){
            return;
    }
    if (dir == 0){
        SetFlip(SDL_FLIP_NONE);
    }else if (dir == 1){
        SetFlip(SDL_FLIP_HORIZONTAL);
    }else if (dir == 2){
        SetFlip(SDL_FLIP_VERTICAL);
    }
    SetClip(anim.sprX * anim.sprW, anim.sprY * anim.sprH,anim.sprW,anim.sprH);
    anim.isFormated = true;
}

int Sprite::GetWidth(){
    return size.x*m_renderData->GetScale().x;
}

int Sprite::GetHeight(){
    return size.y*m_renderData->GetScale().y;
}
int Sprite::GetFrameHeight(){
    return m_renderData->GetClip().h*m_renderData->GetScale().y;
}

int Sprite::GetFrameWidth(){
    return m_renderData->GetClip().w*m_renderData->GetScale().x;
}

