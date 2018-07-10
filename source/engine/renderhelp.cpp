#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
#include <stdio.h>
#include "gamebase.hpp"
#include SDL_LIB_HEADER


bool RenderHelp::RendedTexture(){
    return false;
}




BearTexture* RenderHelp::SurfaceToTexture(SDL_Surface *surface,TextureLoadMethod aliasing){
    if (aliasing.mode == TEXTURE_DEFAULT){
        aliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    if (!surface){
        return nullptr;
    }
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;
    SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ABGR8888, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    SDL_Surface *img_rgba8888 = SDL_CreateRGBSurface(0, surface->w, surface->h, bpp, Rmask, Gmask, Bmask, Amask);
    if (!img_rgba8888){
        return nullptr;
    }

    SDL_SetSurfaceAlphaMod(surface, 0xFF);
    SDL_BlitSurface(surface, NULL, img_rgba8888, NULL);


    BearTexture* ret = Painter::MakeTexture(PointInt(surface->w, surface->h),GL_RGBA,(unsigned char*)img_rgba8888->pixels,aliasing);
    SDL_FreeSurface(img_rgba8888);

    return ret;
}

void RenderHelp::DrawCircleColor(Point p1,float radius,uint8_t r,uint8_t g,uint8_t b,uint8_t a,int sides){

    /*glLoadIdentity();
    glTranslatef(p1.x, p1.y, 0.0f);

    glColor4ub( r,g,b,a );

    glBegin(GL_TRIANGLE_FAN);

      glVertex2f(0,0);
      GLfloat angle;
      for (int i = 0; i <= sides; i++) {
         angle = i * 2.0f * Geometry::PI() / sides;
         glVertex2f( cos(angle) *  radius, sin(angle) *  radius);
      }
    glEnd();*/

}


void RenderHelp::DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a,bool outline,float angle){
    //Painter::DrawSquare(box,r,g,b,a);
    /*glDisable(GL_TEXTURE_2D);
    glLoadIdentity();
    glTranslatef(box.x, box.y, 0.0f);
    if (!outline){
        glBegin( GL_TRIANGLE_FAN );
    }else{
        glBegin( GL_LINE_LOOP );
    }
        glColor4ub( r,g,b,a );
        glVertex2f( 0.0f,0.0f );
        glVertex2f(  box.w, 0.0f );
        glVertex2f( box.w,  box.h );
        glVertex2f( 0.0f,  box.h );
    glEnd();
    DebugHelper::DisplayGlError("DrawSquareColor");*/


}

void RenderHelp::DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a,float thicc){
    /*glLoadIdentity();
    glLineWidth(thicc);
    glColor4ub( r,g,b,a );
    glBegin(GL_LINES);
        glVertex3f(p1.x, p1.y, 0.0f);
        glVertex3f(p2.x, p2.y, 0.0f);
    glEnd();
    glLineWidth(1);
    DebugHelper::DisplayGlError("DrawLineColor");*/
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

Uint32 RenderHelp::FormatRGBA2(int r,int g,int b,int a){
    return a+(b<<8)+(g<<16)+(r<<24);
}

Uint32 RenderHelp::FormatRGBA(int r,int g,int b,int a){
    return r+(g<<8)+(b<<16)+(a<<24);
}

Uint32 RenderHelp::FormatARGB(int a,int r,int b,int g){
    return a+(r<<8)+(g<<16)+(b<<24);
}



