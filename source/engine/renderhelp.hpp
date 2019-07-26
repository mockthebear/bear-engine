#ifndef RENDERH
#define RENDERH
#include "collision.hpp"
#include "../framework/geometry.hpp"
#include "../framework/typechecker.hpp"
#include "../framework/vertex.hpp"
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

        static void DrawPointsColor(std::vector<PointColor> points);
        static void DrawPointColor(PointColor points);

        static void DrawLinesColor(std::vector<LineColor> lines,float thickness=1.0f);

        static void DrawSquaresColor(std::vector<RectColor> rects,bool outline=false);
        static void DrawLineColor(LineColor l, float width = 1.0f);
        static void DrawSquareColor(RectColor box,bool outline=false);

        static void DrawSquareColor(int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b, uint8_t a,bool outline=false,float ang = 0.0f){
            DrawSquareColor(RectColor(Rect(x,y,w,h), BearColor(r,g,b,a),ang),outline);
        }

        static void DrawSquareColor(Rect box ,uint8_t r,uint8_t g,uint8_t b, uint8_t a,bool outline=false,float ang = 0.0f){
            DrawSquareColor(RectColor(box, BearColor(r,g,b,a), ang),outline);
        }

        static void DrawCircleColor(CircleColor &&circle,int sides = 16);
        static void DrawCircleColor(CircleColor &circle,int sides = 16);
        static void DrawCirclesColor(std::vector<CircleColor> circles,int sides=16);


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
