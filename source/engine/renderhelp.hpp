#ifndef RENDERH
#define RENDERH
#include "smarttexture.hpp"
#include "collision.hpp"
#include "../framework/geometry.hpp"
#include "../framework/typechecker.hpp"
#include <functional>

#define COLOR(r,g,b) { (Uint8)(r),(Uint8)(g),(Uint8)(b)}


class RenderHelp{
    public:
        static bool RendedTexture(SDL_Texture* texture,int x,int y,int w,int h,float angle=0,SDL_RendererFlip flip=SDL_FLIP_NONE,Point center=Point(0,0));
        static void DrawLineColorA(int x,int y,int x2,int y2,int r,int g,int b,int a=255);
        static void DrawSquareColorA(Rect box,int r,int g,int b,int a=255,bool outline=false){
            DrawSquareColorA(box.x,box.y,box.w,box.h,r,g,b,a,outline);
        };
        static void DrawSquareColorA(int x,int y,int w,int h,int r,int g,int b,int a=255,bool outline=false);
        static void DrawSquareColorANS(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static void DrawSquareColorUnscaled(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height,std::function<Uint32 (Uint32 , int, int)> F);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height);

        static Uint32 FormatRGBA(int r,int g,int b,int a);
        static Uint32 FormatARGB(int a,int r,int g,int b);


        /*static void DrawCollisionBox(Rect &box,Point offset,int r_color,int g,int b,int alpha=100){
            RenderHelp::DrawSquareColorA(box.x-Camera::pos.x+offset.x,box.y-Camera::pos.y+offset.y,box.w,box.h,r_color,g,b,alpha);
        }*/
        template<class Te> static void DrawCollisionBox(Te &cb,Point offset,int r_color,int g,int b,int alpha=100){
            int maxSize = cb.GetSize();

            SmartTexture T(0,0,maxSize,maxSize,true);
            Uint32 * pixels = T.GetPixels();
            for (int x=0;x<T.getW();x++){
                for (int y=0;y<T.getH();y++){
                    Rect r;
                    r.x = x+cb.x-T.getW()/2;
                    r.y = y+cb.y-T.getH()/2;
                    r.h = 1;
                    r.w = 1;
                    if (Collision::IsColliding(r,cb)){
                        pixels[x + (y*T.getW())] = RenderHelp::FormatRGBA(r_color,g,b,alpha);
                    }else{
                        pixels[x + (y*T.getW())] = RenderHelp::FormatRGBA(r_color,g,b,0);
                    }
                }
            }
            T.UpdateTexture();
            T.Render(PointInt(cb.x-Camera::pos.x-T.getW()/2 + offset.x,cb.y-Camera::pos.y-T.getW()/2 + offset.y));
        };

};


#endif
