#ifndef RENDERH
#define RENDERH
#include "smarttexture.hpp"
#include "../framework/geometry.hpp"
#include <functional>

#define COLOR(r,g,b) { (Uint8)(r),(Uint8)(g),(Uint8)(b)}


class RenderHelp{
    public:
        static bool RendedTexture(SDL_Texture* texture,int x,int y,int w,int h,float angle=0,SDL_RendererFlip flip=SDL_FLIP_NONE,Point center=Point(0,0));
        static void DrawLineColorA(int x,int y,int x2,int y2,int r,int g,int b,int a=255);
        static void DrawSquareColorA(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static void DrawSquareColorANS(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static void DrawSquareColorUnscaled(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height,std::function<Uint32 (Uint32 , int, int)> F);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height);

        static Uint32 FormatRGBA(int r,int b,int g,int a);
        static Uint32 FormatARGB(int a,int r,int b,int g);
};


#endif
