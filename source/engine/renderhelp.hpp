#ifndef RENDERH
#define RENDERH
#include "smarttexture.hpp"
#include "collision.hpp"
#include "../framework/geometry.hpp"
#include "../framework/typechecker.hpp"
#include <functional>

#define COLOR(r,g,b) { (Uint8)(r),(Uint8)(g),(Uint8)(b)}

/**
    @brief Methods to help you rendering stuff
*/
class RenderHelp{
    public:
        /**
            *Given an sdl texture and some informations it will render the same way as class Sprite does
            @param texturePos position of where it should be rendered.
            @param textureSize texture height and width
            @param texture the texture
            @param textureClip clip rect. Leave as Rect(0,0,-1,-1) to dont clip anything
            @param angle rotation. Default = 0
            @param flip option to flip spprite. Default = SDL_FLIP_NONE
            @param center rotation center. Default = Point(-1,-1)  to set as center
        */
        static bool RendedTexture(Point texturePos,Point textureSize,SDL_Texture* texture,RectInt textureClip=RectInt(0,0,-1,-1),float angle=0,SDL_RendererFlip flip=SDL_FLIP_NONE,Point center=Point(-1,-1));
        /**
            *Given an sdl texture and some informations it will render the same way as class Sprite does
            @param texturePos position of where it should be rendered.
            @param textureSize texture height and width
            @param texture the texture
            @param angle rotation. Default = 0
            @param flip option to flip spprite. Default = SDL_FLIP_NONE
            @param center rotation center. Default = Point(-1,-1) to set as center
        */
        static bool RendedTexture(Point texturePos,Point textureSize,SDL_Texture* texture,float angle=0,SDL_RendererFlip flip=SDL_FLIP_NONE,Point center=Point(-1,-1)){
            return RendedTexture(texturePos,textureSize,texture,RectInt(0,0,-1,-1),angle,flip,center);
        }
        /**
            *Draw a single pixel line. Does not be affected by scaling in is thickness
            @param x position x
            @param y position y
            @param x2 position x2
            @param y2 position y2
            @param r red
            @param g green
            @param b blue
            @param a alpha = 255
        */
        static void DrawLineColorA(int x,int y,int x2,int y2,int r,int g,int b,int a=255);
        static void DrawSquareColorA(Rect box,int r,int g,int b,int a=255,bool outline=false){
            DrawSquareColorA(box.x,box.y,box.w,box.h,r,g,b,a,outline);
        };

        static void DrawSquareColorA(int x,int y,int w,int h,int r,int g,int b,int a=255,bool outline=false);

        static void DrawSquareColor(int x,int y,int w,int h,int r,int g,int b,int a=255,bool outline=false){
            DrawSquareColorA( x, y, w, h, r, g, b, a,outline);
        }

        static void DrawSquareColorANS(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static void DrawSquareColorUnscaled(int x,int y,int w,int h,int r,int g,int b,int a=255);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height,std::function<Uint32 (Uint32 , int, int)> F);
        static SmartTexture *GeneratePatternTexture(int x,int y,int width,int height);

        /**
            * Given the components r,g,b,a return the color in RGBA format
            @param r red
            @param g green
            @param b blue
            @param a alpha
        */
        static Uint32 FormatRGBA2(int r,int g,int b,int a);
        static Uint32 FormatRGBA(int r,int g,int b,int a);

        /**
            * Given the components a,r,g,b return the color in ARGB format
            @param a alpha
            @param r red
            @param g green
            @param b blue

        */
        static Uint32 FormatARGB(int a,int r,int g,int b);

        /**
            Given an color in ARGB format, get its Red componet
            @param c Color
            @return component
        */
        static uint8_t GetR(uint32_t c);

        /**
            Given an color in ARGB format, get its Green componet
            @param c Color
            @return component
        */
        static uint8_t GetG(uint32_t c);

        /**
            Given an color in ARGB format, get its Blue componet
            @param c Color
            @return component
        */
        static uint8_t GetB(uint32_t c);

        /**
            Given an color in ARGB format, get its Alpha componet
            @param c Color
            @return component
        */
        static uint8_t GetA(uint32_t c);

        /**
            Given an rect, its drawn relative to the camera and an offset, the colission box
            @param box The rect/colission box
            @param offset any offset in Point format
            @param r_color Red
            @param g_color Green
            @param b_color Blue
            @param alpha Alpha, default = 100.
        */
        static void DrawCollisionBox(Rect &box,Point offset,int r_color,int g_color,int b_color,int alpha=100){
            RenderHelp::DrawSquareColorA(box.x-Camera::pos.x+offset.x,box.y-Camera::pos.y+offset.y,box.w,box.h,r_color,g_color,b_color,alpha);
        }
        static void DrawCollisionBox(Rect &box,int r_color,int g_color,int b_color,int alpha=100){
            RenderHelp::DrawSquareColorA(box.x-Camera::pos.x,box.y-Camera::pos.y,box.w,box.h,r_color,g_color,b_color,alpha);
        }
        template<class Te> static void DrawCollisionBox2(Te &cb,Point offset,int r_color,int g,int b,int alpha=100){
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
                        pixels[x + (y*T.getW())] = RenderHelp::FormatRGBA(0,0,0,0);
                    }
                }
            }
            T.UpdateTexture();
            T.Render(PointInt(cb.x-Camera::pos.x-T.getW()/2 + offset.x,cb.y-Camera::pos.y-T.getW()/2 + offset.y));
        };

};


#endif
