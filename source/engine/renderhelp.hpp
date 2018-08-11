#ifndef RENDERH
#define RENDERH
#include "collision.hpp"
#include "../framework/geometry.hpp"
#include "../framework/typechecker.hpp"
#include <functional>


/**
    @brief Methods to help you rendering stuff
*/


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

        static void DrawLinesColor(std::vector<Point> lines,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,float thickness=1.0f);
        static void DrawSquaresColor(std::vector<Rect> rects,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false);


        static void DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,float thickness=1.0f);


        static void DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false);

        static void DrawSquareColor_lua(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false){
            DrawSquareColor(box,r,g,b,a,outline);
        }

        static void DrawSquareColor(int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,bool outline=false){
            DrawSquareColor(Rect(x,y,w,h),r,g,b,a,outline);
        }

        static void DrawCircleColor(Point p1,float radius,uint8_t r,uint8_t g,uint8_t b,uint8_t a=255,int sides = 16);


        static BearTexture* SurfaceToTexture(SDL_Surface *surface,TextureLoadMethod aliasing=TEXTURE_LINEAR);

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

        static uint8_t GetA(uint32_t c);


};


#endif
