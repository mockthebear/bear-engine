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

void RenderHelp::DrawCirclesColor(std::vector<CircleColor> circles,int sides){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();

    for (auto &it : circles){
        vertexBuffer->AddCircle(it, sides);
    }
    Painter::DrawVertex(vertexBuffer,GL_TRIANGLES, GL_IS_AUTOBOUND);
    vertexBuffer->clear();
}


void RenderHelp::DrawCircleColor(CircleColor &circle,int sides){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();

    vertexBuffer->AddCircle(circle, sides);

    Painter::DrawVertex(vertexBuffer,GL_TRIANGLES,   GL_IS_AUTOBOUND);
    vertexBuffer->clear();
}

void RenderHelp::DrawCircleColor(CircleColor &&circle,int sides){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();

    vertexBuffer->AddCircle(circle, sides);

    Painter::DrawVertex(vertexBuffer,GL_TRIANGLES,   GL_IS_AUTOBOUND);
    vertexBuffer->clear();
}


void RenderHelp::DrawSquareColor(RectColor box,bool outline){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;

    transformations.size = box.GetSize();
    transformations.translation = box.GetPos();
    transformations.SetAngle(box.angle);
    transformations.center = Point(box.w/2.0f, box.h/2.0f);

    transformations.forwardClip = Rect(-1, -1, -1, -1);


    vertexBuffer->AddRect(transformations, box.colors, outline);

    Painter::DrawVertex(vertexBuffer,outline ? GL_LINE_LOOP : GL_TRIANGLES,   GL_IS_AUTOBOUND);
    vertexBuffer->clear();
}

void RenderHelp::DrawSquaresColor(std::vector<RectColor> rects,bool outline){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;

    if (rects.size() == 0){
        return;
    }

    transformations.forwardClip = Rect(-1, -1, -1, -1);

    for (auto &it : rects){
        vertexBuffer->AddRect(it, outline);
    }


    Painter::DrawVertex(vertexBuffer,outline ? GL_LINE_LOOP : GL_TRIANGLES,   GL_IS_AUTOBOUND);

    vertexBuffer->clear();

    DisplayGlError("DrawSquaresColor");
}

void RenderHelp::DrawPointColor(PointColor point){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;
    transformations.forwardClip = Rect(-1, -1, -1, -1);
    vertexBuffer->AddVertice(Vertex(point, point.color));
    Painter::DrawVertex(vertexBuffer,GL_POINTS,GL_IS_AUTOBOUND);
    vertexBuffer->clear();
}

void RenderHelp::DrawPointsColor(std::vector<PointColor> points){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;

    if (points.size() == 0){
        return;
    }

    transformations.forwardClip = Rect(-1, -1, -1, -1);



    for (auto &it : points){
        vertexBuffer->AddVertice(Vertex(it, it.color));
    }

    Painter::DrawVertex(vertexBuffer,GL_POINTS,GL_IS_AUTOBOUND);

    vertexBuffer->clear();
}


void RenderHelp::DrawLinesColor(std::vector<LineColor> lines,float thickness){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;

    transformations.forwardClip = Rect(-1, -1, -1, -1);

    if (lines.size() == 0){
        return;
    }

    for (auto &it : lines){
        vertexBuffer->AddLine(it);
    }

    glLineWidth(thickness);
    Painter::DrawVertex(vertexBuffer,GL_LINES,GL_IS_AUTOBOUND);
    glLineWidth(1.0f);
    vertexBuffer->clear();
}

void RenderHelp::DrawLineColor(LineColor l, float width){
    static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
    static AdvancedTransformations transformations;

    transformations.forwardClip = Rect(-1, -1, -1, -1);


    vertexBuffer->AddLine(l);
    glLineWidth(width);
    Painter::DrawVertex(vertexBuffer,GL_LINES,GL_IS_AUTOBOUND);
    glLineWidth(1.0f);
    vertexBuffer->clear();
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



