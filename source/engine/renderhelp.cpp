#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
#include <stdio.h>
#include "gamebase.hpp"
#include SDL_LIB_HEADER

void RenderHelp::DrawSquareColorUnscaled(int x,int y,int w,int h,int r,int g,int b,int a){
    SDL_SetRenderDrawColor(BearEngine->GetRenderer(), r, g,b, a);
    SDL_Rect rectangle;
    rectangle.x = ( x );
    rectangle.y = ( y );
    rectangle.w = ( w );
    rectangle.h = ( h );
    SDL_RenderFillRect(BearEngine->GetRenderer(), &rectangle);

}

bool RenderHelp::RendedTexture(Point texturePos,Point textureSize,SDL_Texture* texture,RectInt textureClip,float angle,SDL_RendererFlip flip,Point center_){
    if (!texture){
        return false;
    }
    SDL_Point center;
    if (textureClip.x == -1 || textureClip.y == -1){
        textureClip.w = textureSize.x;
        textureClip.h = textureSize.y;
    }
    SDL_Rect clipRect = {textureClip.x,textureClip.y,textureClip.w,textureClip.h};
    center.x = center_.x;
    center.y = center_.y;
    if (center.x == -1 || center.y == -1){
        center.x = textureSize.x/2.0;
        center.y = textureSize.y/2;0;
    }
    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
    SDL_Rect rectangle;
    rectangle.x = ( texturePos.x*scaleRatioW  )+ ScreenManager::GetInstance().GetOffsetW();
    rectangle.y = ( texturePos.y*scaleRatioH  )+ ScreenManager::GetInstance().GetOffsetH();
    rectangle.w = ( textureSize.x*scaleRatioW  );
    rectangle.h = ( textureSize.y*scaleRatioH  );
    return SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,&clipRect,&rectangle,angle,&center,flip) == 0;
}

void RenderHelp::DrawSquareColorA(int x,int y,int w,int h,int r,int g,int b,int a,bool outline){

    SDL_SetRenderDrawColor(BearEngine->GetRenderer(), r, g,b, a);
    SDL_Rect rectangle;
    double scaleRatioW = (ScreenManager::GetInstance().GetScaleRatioW());
    double scaleRatioH = (ScreenManager::GetInstance().GetScaleRatioH());
    rectangle.x = ( x*scaleRatioW  )+ ScreenManager::GetInstance().GetOffsetW();
    rectangle.y = ( y*scaleRatioH  )+ ScreenManager::GetInstance().GetOffsetH();
    rectangle.w = ( w*scaleRatioW  );
    rectangle.h = ( h*scaleRatioH  );
    if (!outline){
        SDL_RenderFillRect(BearEngine->GetRenderer(), &rectangle);
    }else{
        SDL_RenderDrawRect (BearEngine->GetRenderer(), &rectangle);
    }

}

void RenderHelp::DrawSquareColorANS(int x,int y,int w,int h,int r,int g,int b,int a){

    SDL_SetRenderDrawColor(BearEngine->GetRenderer(), r, g,b, a);
    SDL_Rect rectangle;
    rectangle.x = ( x  );
    rectangle.y = ( y );
    rectangle.w = ( w  );
    rectangle.h = ( h  );
    SDL_RenderFillRect(BearEngine->GetRenderer(), &rectangle);

}

void RenderHelp::DrawLineColorA(int x,int y,int w,int h,int r,int g,int b,int a){
    double scaleRatioW = (ScreenManager::GetInstance().GetScaleRatioW());
    double scaleRatioH = (ScreenManager::GetInstance().GetScaleRatioH());
    SDL_SetRenderDrawColor(BearEngine->GetRenderer(), r, g,b, a);
    SDL_RenderDrawLine(BearEngine->GetRenderer(),
                       x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW(),
                       y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH(),
                       w*scaleRatioW + ScreenManager::GetInstance().GetOffsetW(),
                       h*scaleRatioH + ScreenManager::GetInstance().GetOffsetH());
}

SmartTexture *RenderHelp::GeneratePatternTexture(int x,int y,int w,int h){
    SDL_Texture *t = SDL_CreateTexture( BearEngine->GetRenderer(),SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
    Uint32 * pixels = new Uint32[w * h];
    DebugHelper::AssertAlloc(pixels,WHERE_ARG);
    SDL_SetTextureBlendMode( t, SDL_BLENDMODE_BLEND );

    for (int y = 0;y < h;y++){
        for (int x=0;x<w;x++){
             pixels[y * h + x] = RenderHelp::FormatRGBA(rand()%255,rand()%255,rand()%255, rand()%255);
        }
    }
    SDL_UpdateTexture(t, NULL, pixels, w * sizeof(Uint32));
	return new SmartTexture(t,pixels,x,y,h,w);

}

SmartTexture *RenderHelp::GeneratePatternTexture(int x,int y,int w,int h,std::function<Uint32 (Uint32 , int, int)> F){
    SDL_Texture *t = SDL_CreateTexture( BearEngine->GetRenderer(),SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
    Uint32 * pixels = new Uint32[w * h];
    if (pixels == NULL){
        return NULL;
    }

    SDL_SetTextureBlendMode( t, SDL_BLENDMODE_BLEND );
    for (int y = 0;y < h;y++){
        for (int x=0;x<w;x++){
             pixels[y * h + x] = F(pixels[y * h + x],x,y);
        }
    }
    SDL_UpdateTexture(t, NULL, pixels, w * sizeof(Uint32));
	return new SmartTexture(t,pixels,x,y,h,w);
}

uint8_t RenderHelp::GetR(uint32_t r){
    return r&0xff;
}

uint8_t RenderHelp::GetG(uint32_t r){
    return (r&0xff00) >> 8;
}

uint8_t RenderHelp::GetB(uint32_t r){
    return (r&0xff0000) >> 16;
}

uint8_t RenderHelp::GetA(uint32_t r){
    return (r&0xff000000) >> 24;
}


Uint32 RenderHelp::FormatRGBA(int r,int g,int b,int a){

    return r+(g<<8)+(b<<16)+(a<<24);

}

Uint32 RenderHelp::FormatARGB(int a,int r,int b,int g){
    return a+(r<<8)+(g<<16)+(b<<24);
}
