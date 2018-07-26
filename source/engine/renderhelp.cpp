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


    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static BasicRenderDataPtr renderData         = std::make_shared<BasicRenderData>();


    Circle c(p1.x,p1.x,radius);
    renderData->position = p1;
    renderData->size.x = radius;
    renderData->size.y = radius;
    renderData->color = BearColor(r,g,b,a);

    vertexBuffer->clear();

    vertexBuffer->vertexes.Generate(c,sides);
    vertexBuffer->SetupVertexes();


    Painter::DrawVertex(vertexBuffer,renderData,GL_TRIANGLE_FAN);
}


void RenderHelp::DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a,bool outline,float angle){

    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static BasicRenderDataPtr renderData         = std::make_shared<BasicRenderData>();

    renderData->position = box.GetPos();
    renderData->size.x = box.w;
    renderData->size.y = box.h;
    renderData->SetAngle(angle);
    renderData->color = BearColor(r,g,b,a);

    vertexBuffer->clear();

    vertexBuffer->vertexes.Generate(box);
    vertexBuffer->SetupVertexes();

    if (!outline){
        Painter::DrawVertex(vertexBuffer,renderData,GL_TRIANGLE_FAN);
    }else{
        Painter::DrawVertex(vertexBuffer,renderData,GL_LINE_LOOP);
    }
}

void RenderHelp::DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a,float thicc){

    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static BasicRenderDataPtr renderData         = std::make_shared<BasicRenderData>();

    renderData->position = p1;
    renderData->size.x = (p2.x - p1.x);
    renderData->size.y = (p2.y - p1.y);
    renderData->color = BearColor(r,g,b,a);

    if (!Painter::CanSupport(SUPPORT_ORTOHOVERTEX)){
        renderData->SetAngle( atan( (p2.y - p1.y) / (p2.x - p1.x) ) );
    }

    vertexBuffer->clear();

    vertexBuffer->vertexes.Generate(p1,p2);
    vertexBuffer->SetupVertexes();


    Painter::DrawVertex(vertexBuffer,renderData,GL_LINES);
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



