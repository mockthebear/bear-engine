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

class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        Framebuffer = 0;
        renderedTexture = 0;
        depthrenderbuffer = 0;
        scale = Point(1.0f,1.0f);
    };
    bool Generate(int w,int h);
    bool Bind();
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){scale = p;};

    GLuint vbo_fbo_vertices;
    GLuint renderedTexture;
    GLuint Framebuffer;
    GLuint depthrenderbuffer;
    static GLint lastbuffer;
    Point scale;
};
class RenderHelp{
    public:

        static bool RendedTexture();
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
        static void DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,float thickness=1.0f);


        static void DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false,float angle=0);

        static void DrawSquareColorA(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false,float angle=0){
            DrawSquareColor(box,r,g,b,a,outline,angle);
        };

        static void DrawSquareColorA(int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false,float angle=0){
            DrawSquareColorA(Rect(x,y,w,h),r,g,b,a,outline,angle);
        }

        static void DrawCircleColor(Point p1,float radius,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,int sides = 16);


        static BearTexture* SurfaceToTexture(SDL_Surface *surface,TextureLoadMethod aliasing=TEXTURE_LINEAR);

        static BearTexture* CreateTexture(int width,int height,TextureLoadMethod aliasing=TEXTURE_LINEAR);


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
        /*static void DrawCollisionBox(Rect &box,Point offset,int r_color,int g_color,int b_color,int alpha=100){
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
        template<class Te> static void DrawCollisionBox3(Te &cb,float angle,Point offset,int r_color,int g,int b,int alpha=100){
            int maxSize = cb.GetSize()*4.0;

            SmartTexture T(0,0,maxSize,maxSize,true);
            Uint32 * pixels = T.GetPixels();
            for (int x=0;x<T.getW();x++){
                for (int y=0;y<T.getH();y++){
                    Rect r;
                    r.x = x+cb.x-T.getW()/2;
                    r.y = y+cb.y-T.getH()/2;
                    r.h = 1;
                    r.w = 1;
                    if (Collision::IsColliding(r,cb,0,angle)){
                        pixels[x + (y*T.getW())] = RenderHelp::FormatRGBA(r_color,g,b,alpha);
                    }else{
                        pixels[x + (y*T.getW())] = RenderHelp::FormatRGBA(0,0,0,0);
                    }
                }
            }
            T.UpdateTexture();
            T.Render(PointInt(cb.x-Camera::pos.x-T.getW()/2 + offset.x,cb.y-Camera::pos.y-T.getW()/2 + offset.y));
        };

        */

};


#endif
