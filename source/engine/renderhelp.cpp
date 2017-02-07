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

bool RenderHelp::RendedTexture(SDL_Texture* texture,int x,int y,int w,int h,float angle,SDL_RendererFlip flip,Point center_){
    SDL_Point center;
    center.x = center_.x;
    center.y = center_.y;
    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
    SDL_Rect rectangle;
    rectangle.x = ( x*scaleRatioW  )+ ScreenManager::GetInstance().GetOffsetW();
    rectangle.y = ( y*scaleRatioH  )+ ScreenManager::GetInstance().GetOffsetH();
    rectangle.w = ( w*scaleRatioW  );
    rectangle.h = ( h*scaleRatioH  );
    return SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,NULL,&rectangle,angle,&center,flip) == 0;
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
Uint32 RenderHelp::FormatRGBA(int r,int g,int b,int a){

    return a+(b<<8)+(g<<16)+(r<<24);
}

Uint32 RenderHelp::FormatARGB(int a,int r,int b,int g){

    Uint32 R,G,B;
    R = a << (24);
    G = r << 16;
    B = g << 8;
    return R+G+B+b;
}
