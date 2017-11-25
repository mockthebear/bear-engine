#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
#include <stdio.h>
#include "gamebase.hpp"
#include SDL_LIB_HEADER


bool RenderHelp::RendedTexture(){
    return false;
}

GLuint powerOfTwo( GLuint num )
{
    if( num != 0 )
    {
        num--;
        num |= (num >> 1);
        num |= (num >> 2);
        num |= (num >> 4);
        num |= (num >> 8);
        num |= (num >> 16);
        num++;
    }
    return num;
}

BearTexture* RenderHelp::CreateTexture(int width,int height,TextureLoadMethod aliasing){
    GLuint texId=0;
    glGenTextures(1, &texId);
    if (texId == 0){
        return nullptr;
    }
    glBindTexture(GL_TEXTURE_2D, texId);

    unsigned int pow_w =  powerOfTwo(width);
    unsigned int pow_h =  powerOfTwo(height);

    aliasing.ApplyFilter();


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pow_w, pow_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    DebugHelper::DisplayGlError();
    BearTexture *t2 = new BearTexture(texId,pow_w, pow_h,GL_RGBA);
    t2->size_w = width;
    t2->size_h = height;
    return t2;
}

BearTexture* RenderHelp::SurfaceToTexture(SDL_Surface *surface,TextureLoadMethod aliasing){
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

    GLuint texId=0;
    glGenTextures(1, &texId);
    if (texId == 0){
        return nullptr;
    }
    glBindTexture(GL_TEXTURE_2D, texId);

    unsigned int pow_w =  powerOfTwo(surface->w);
    unsigned int pow_h =  powerOfTwo(surface->h);

    aliasing.ApplyFilter();


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pow_w, pow_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, img_rgba8888->pixels);


    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(img_rgba8888);
    BearTexture *t = new BearTexture(texId,pow_w, pow_h,GL_RGBA);
    t->size_w = surface->w;
    t->size_h = surface->h;
    return t;
}

void RenderHelp::DrawCircleColor(Point p1,float radius,uint8_t r,uint8_t g,uint8_t b,uint8_t a,int sides){
    #ifdef RENDER_OPENGL
    glLoadIdentity();
    glTranslatef(p1.x, p1.y, 0.0f);

    glColor4ub( r,g,b,a );

    glBegin(GL_TRIANGLE_FAN);

      glVertex2f(0,0);
      GLfloat angle;
      for (int i = 0; i <= sides; i++) {
         angle = i * 2.0f * Geometry::PI() / sides;
         glVertex2f( cos(angle) *  radius, sin(angle) *  radius);
      }
    glEnd();

    glPopMatrix();
    #endif // RENDER_OPENGL
}


void RenderHelp::DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a,bool outline,float angle){
    #ifdef RENDER_OPENGL
    glLoadIdentity();
    glTranslatef(box.x, box.y, 0.0f);
    if (!outline){
        glBegin( GL_QUADS );
    }else{
        glBegin( GL_LINE_LOOP );
    }
        glColor4ub( r,g,b,a );
        glVertex2f( 0,0 );
        glVertex2f(  box.w, 0 );
        glVertex2f( box.w,  box.h );
        glVertex2f( 0,  box.h );
    glEnd();
    glPopMatrix();
    #else

    #endif

}

void RenderHelp::DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a,float thicc){
    #ifndef RENDER_OPENGL
    GLfloat line[] = {
                     0,0,0,
                     100,100,0
                  };

    GLfloat colors[] = {
                            1.0f, 0.0f, 0.0f, 1.0f,
                            0.0f, 1.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f, 1.0f
                        };
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //glShadeModel(GL_SMOOTH);
    //glVertexPointer(3, GL_FLOAT, 0, line);
    glColorPointer(4, GL_FLOAT, 0, colors);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_LINES, 0, 2);
    glFlush();
    #else
        glLoadIdentity();
        glLineWidth(thicc);
        glColor4ub( r,g,b,a );
        glBegin(GL_LINES);
            glVertex3f(p1.x, p1.y, 0.0);
            glVertex3f(p2.x, p2.y, 0);
        glEnd();
        glLineWidth(1);
        glPopMatrix();
    #endif

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


/**
    TARGET TEXTURE
**/

GLint TargetTexture::lastbuffer = 0;

void TargetTexture::Render(Point pos){

    #ifdef RENDER_OPENGL
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glEnable(GL_TEXTURE_2D);


        float quadWidth = size_w;
        float quadHeight = size_h;


    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glTranslatef(
                (pos.x  + quadWidth / 2.f  ),
                (pos.y + quadHeight/ 2.f  ),
        0.f );

    glBegin( GL_QUADS );
        glTexCoord2f(  0.0f , 1.0f ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f(  1.0f , 1.0f ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f(  1.0f , 0.0f ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  0.0f , 0.0f ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, 0 );
    glPopMatrix();
    #endif // RENDER_OPENGL
}

bool TargetTexture::Bind(){
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    Point gameCanvas = ScreenManager::GetInstance().GetGameSize();

    #ifdef RENDER_OPENGL
    glViewport(0, 0, size_w, size_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size_w, size_w, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    #endif // RENDER_OPENGL


    return true;
}


bool TargetTexture::UnBind(){
    if (lastbuffer == 0){
        lastbuffer = ScreenManager::GetInstance().GetDefaultFrameBuffer();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, lastbuffer);
    lastbuffer = 0;
    ScreenManager::GetInstance().ResetViewPort();
    return true;
}

bool TargetTexture::Generate(int wa,int ha){
    size_w = w = wa;
    size_h = h = ha;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastbuffer);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wa, ha, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Depth buffer */
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, wa,ha);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /* Framebuffer to link everything together */
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderedTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);



    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLfloat fbo_vertices[] = {
        -1, 1,
        1,  1,
        -1, -1,
        1,  -1,
    };
    glGenBuffers(1, &vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindFramebuffer(GL_FRAMEBUFFER, lastbuffer);
    lastbuffer = 0;
    return true;

}



