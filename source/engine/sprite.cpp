#include "sprite.hpp"
#include "gamebase.hpp"
#include <math.h>
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "renderhelp.hpp"
#include "smarttexture.hpp"
#include "../framework/resourcemanager.hpp"


std::unordered_map<std::string, SDL_Texture*> Sprite::assetTable;

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
    texture = NULL;
    repeat = 1;
    hasCenter = false;
    fname = "";
    over = 0;
    OUTR = OUTB = OUTG = 255;
    sprFlip = SDL_FLIP_NONE;
}

Sprite::Sprite(SDL_Texture* texture_,std::string name){
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    frameCount = 1;
    currentFrame = PointInt(0,0);
    timeElapsed = 0;
    hasCenter = false;
    frameTime = 0;
    repeat = 1;
    texture = texture_;
    fname = name;
    over = 0;
    Uint32 format;
    int acess;
    if (texture != NULL){
        if (SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h) < 0){
            texture = NULL;
        }
        SetClip(0,0,dimensions.w,dimensions.h);
    }
    if (name != "" and !assetTable[name]){
        assetTable[name] = texture_;
    }


    OUTR = OUTB = OUTG = 255;
    SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    SetFrame(0);
    sprFlip = SDL_FLIP_NONE;
}

Sprite::Sprite(const char *file,ColorReplacer &r,bool replacer,int fcount,float ftime,int rep){
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    frameCount = fcount;
    currentFrame = PointInt(0,0);
    timeElapsed = 0;
    repeat = rep;
    hasCenter = false;
    frameTime = ftime;
    texture = NULL;
    fname = file;
    texture = ColorReplace(std::string(file),r,replacer);
    if (texture){
        Uint32 format;
        int acess;
        SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
        scaleX = scaleY = 1;
        SetClip(0,0,dimensions.w,dimensions.h);
    }
    over = 0;
    OUTR = OUTB = OUTG = 255;
    if (fcount == 1){
        SetGrid(GetWidth(),GetHeight());
    }else{
        SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    }
    SetFrame(0);
    SetAlpha(255);
    sprFlip = SDL_FLIP_NONE;

}

Sprite::Sprite(char *file,int fcount,float ftime,int rep){
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    frameCount = fcount;
    currentFrame = PointInt(0,0);
    timeElapsed = 0;
    repeat = rep;
    hasCenter = false;
    frameTime = ftime;
    texture = NULL;
    fname = file;
    Open(file);
    over = 0;
    OUTR = OUTB = OUTG = 255;
    if (fcount == 1){
        SetGrid(GetWidth(),GetHeight());
    }else{
        SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    }
    SetFrame(0);
    SetAlpha(255);
    sprFlip = SDL_FLIP_NONE;
}

Sprite::Sprite(SDL_RWops* file,std::string name,int fcount,float ftime,int rep){
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    frameCount = fcount;
    currentFrame = PointInt(0,0);
    timeElapsed = 0;
    repeat = rep;
    hasCenter = false;
    frameTime = ftime;
    texture = NULL;
    fname = name;
    Open(file,name);
    over = 0;
    OUTR = OUTB = OUTG = 255;
    if (fcount == 1){
        SetGrid(GetWidth(),GetHeight());
    }else{
        SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    }
    SetFrame(0);
    SetAlpha(255);
    sprFlip = SDL_FLIP_NONE;
}

Sprite::Sprite(const char *file,int fcount,float ftime,int rep){
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    frameCount = fcount;
    currentFrame = PointInt(0,0);
    timeElapsed = 0;
    fname = file;
    frameTime = ftime;
    texture = NULL;
    repeat = rep;
    hasCenter = false;
    over = 0;
    Open((char *)file);


    OUTR = OUTB = OUTG = 255;
    if (fcount == 1){
        SetGrid(GetWidth(),GetHeight());
    }else{
        SetGrid(GetWidth()/frameCount,GetWidth()/frameCount);
    }
    SetFrame(0);
    SetAlpha(255);
    sprFlip = SDL_FLIP_NONE;
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
    if (fname != ""){
        return NULL;
    }
    return Open((char*)fname.c_str(),true);
}

void Sprite::Clear(){
    std::cout << "[Sprite::Clear] Cleaning..";
    for (auto &it : assetTable){
        SDL_DestroyTexture( it.second );
    }
    assetTable.clear();
    std::cout << ".\n";
}

SDL_Texture* Sprite::Preload(char *file,bool adjustDir){
    std::string stdnamee(file);
    if (adjustDir){
        stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    }
    if (assetTable.find(stdnamee) == assetTable.end()){
        SDL_Texture* texture = IMG_LoadTexture(BearEngine->GetRenderer(),stdnamee.c_str());
        if (texture != NULL){
            Uint32 format;
            int acess;
            SDL_Rect dimensions;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
            assetTable[stdnamee] = texture;
            return texture;
        }else{
           Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s]",stdnamee.c_str()));
        }
    }
    return NULL;
}

SDL_Texture* Sprite::Preload(SDL_RWops* rw,std::string name){
    if (assetTable.find(name) == assetTable.end()){
        SDL_Texture* texture = NULL;
        SDL_Surface *aux_surface = IMG_Load_RW(rw, 1);
        if (aux_surface){
            texture =  SDL_CreateTextureFromSurface(BearEngine->GetRenderer(),aux_surface);
            SDL_FreeSurface(aux_surface);
        }
        if (texture != NULL){
            Uint32 format;
            int acess;
            SDL_Rect dimensions;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
            assetTable[name] = texture;
            return texture;
        }else{
           Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s]",name.c_str()));
        }
    }
    return NULL;
}
SDL_Texture* Sprite::ColorReplace(std::string fileName,ColorReplacer &r,bool replaceOnAssets){
    if (fileName == ""){
        return nullptr;
    }
    SDL_Surface *surface = nullptr;
    if (fileName.find(":")!=std::string::npos){
        surface = IMG_Load_RW(ResourceManager::GetInstance().GetFile(fileName),1);
    }else{
        surface = IMG_Load( fileName.c_str() );
    }
    if (surface){
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
                if (replaceOnAssets){
                    if (assetTable[fileName]){
                        SDL_DestroyTexture(assetTable[fileName]);
                    }
                    assetTable[fileName] = ret;
                }
                delete finalSmart;
                return ret;

            }
        }

    }

    return nullptr;
}



SDL_Texture* Sprite::Open(char *file,bool force){
    scaleX = scaleY = 1;
    std::string stdnamee(file);

    if (stdnamee.find(":")!=std::string::npos){
        return Open(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee,force);
    }

    stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    if (assetTable.find(stdnamee) == assetTable.end() or force){

        texture = IMG_LoadTexture(BearEngine->GetRenderer(),stdnamee.c_str());
        if (texture != NULL){
            Uint32 format;
            int acess;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);

            SetClip(0,0,dimensions.w,dimensions.h);
            assetTable[stdnamee] = texture;
            return texture;
        }else{
           Console::GetInstance().AddTextInfo(utils::format("Cannot load sprite [%s]",stdnamee.c_str()));
           return NULL;
        }
    }else{
        std::unordered_map<std::string, SDL_Texture*>::iterator it = assetTable.find(stdnamee);
        texture = (*it).second;
        if (texture){
            Uint32 format;
            int acess;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
            SetClip(0,0,dimensions.w,dimensions.h);
        }
        return texture;
    }


}

SDL_Texture* Sprite::Open(SDL_RWops* file,std::string name,bool force){
    scaleX = scaleY = 1;
    if (assetTable.find(name) == assetTable.end() or force){
        SDL_Surface *aux_surface = IMG_Load_RW(file, 1);
        texture = NULL;
        if (aux_surface){
            texture =  SDL_CreateTextureFromSurface(BearEngine->GetRenderer(),aux_surface);
            SDL_FreeSurface(aux_surface);
        }
        if (texture != NULL){
            Uint32 format;
            int acess;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
            SetClip(0,0,dimensions.w,dimensions.h);
            assetTable[name] = texture;
            return texture;
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
    }else{
        std::unordered_map<std::string, SDL_Texture*>::iterator it = assetTable.find(name);
        texture = (*it).second;
        if (texture){
            Uint32 format;
            int acess;
            SDL_QueryTexture(texture, &format,&acess,&dimensions.w,&dimensions.h);
            SetClip(0,0,dimensions.w,dimensions.h);
        }
        return texture;
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
    SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
}
void Sprite::Render(int x,int y,double angle){
    SDL_Rect dimensions2;

    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    dimensions2.x = x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
    dimensions2.y = y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
    dimensions2.h = clipRect.h*scaleRatioH*scaleY;
    dimensions2.w = clipRect.w*scaleRatioW*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
}


void Sprite::RawRender(int x,int y,double angle){
    SDL_Rect dimensions2;
    dimensions2.x = x;
    dimensions2.y = y;
    dimensions2.h = clipRect.h*scaleY;
    dimensions2.w = clipRect.w*scaleX;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip); //wat
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
