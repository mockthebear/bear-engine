#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
#include <stdio.h>
#include "gamebase.hpp"
#include SDL_LIB_HEADER
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader RenderHelp::textureShader;
Shader RenderHelp::polygonShader;
bool RenderHelp::AngleInDegree = true;

bool RenderHelp::RenderTexture(BearTexture *t, Point pos,Rect clipRect, float rotation,Point scale, SDL_RendererFlip flip,BearColor recolor){
    #ifdef RENDER_OPENGL
        #ifdef GL2
            glLoadIdentity();
            glEnable(GL_TEXTURE_2D);
            glColor4f(OUTR, OUTG, OUTB, m_alpha);
            float w = dimensions.w;
            float h = dimensions.h;

            texLeft = clipRect.x / (float)w;
            texRight =  ( clipRect.x + clipRect.w ) / (float)w;
            texTop = clipRect.y / (float)h;
            texBottom = ( clipRect.y + clipRect.h ) / (float)h;

            GLfloat quadWidth = clipRect.w ;
            GLfloat quadHeight = clipRect.h ;

            glScalef(scaleX , scaleY , 1.0f);

            glTranslatef(
                (pos.x * (1.0f/scaleX)  + quadWidth  / 2.f  ) + (- center.x* (scaleX)  + center.x),
                (pos.y * (1.0f/scaleY)  + quadHeight / 2.f  ) + (- center.y* (scaleY)  + center.y),
            0.f);

            glRotatef( angle, 0.f, 0.f, 1.f );

            glBindTexture( GL_TEXTURE_2D, textureShred.get()->id );
            if ((sprFlip&SDL_FLIP_HORIZONTAL) != 0){
                float holder =  texLeft;
                texLeft = texRight;
                texRight = holder;
            }
            if ((sprFlip&SDL_FLIP_VERTICAL) != 0){
                float holder =  texTop;
                texTop = texBottom;
                texBottom = holder;
            }

            glBegin( GL_TRIANGLE_FAN );
                glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
                glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
                glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
                glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
            glEnd();
        #else
            if (AngleInDegree){
                rotation = Geometry::toRad(rotation);
            }

            static GLuint VAO;
            static GLuint VBO;
            static bool made = false;

            if (!t || t->id == 0){
                return false;
            }

            float w = t->w;
            float h = t->h;
            if (clipRect.w < 0){
                clipRect.w = t->size_w;
            }
            if (clipRect.h < 0){
                clipRect.h = t->size_h;
            }
            glm::vec2 size(clipRect.w,clipRect.h);
            glm::mat4 model(1.0f);
            glm::mat4 projection;

            float texLeft = clipRect.x / (float)w; //0
            float texRight =  ( clipRect.x + clipRect.w ) / (float)w; //1
            float texTop = clipRect.y / (float)h; //0
            float texBottom = ( clipRect.y + clipRect.h ) / (float)h; //1


            if ((flip&SDL_FLIP_HORIZONTAL) != 0){
                float holder =  texLeft;
                texLeft = texRight;
                texRight = holder;
            }
            if ((flip&SDL_FLIP_VERTICAL) != 0){
                float holder =  texTop;
                texTop = texBottom;
                texBottom = holder;
            }

            GLfloat vertices[] = {


                // Pos      // Tex
                0.0f, 0.0f, texLeft, texTop,
                0.0f, 1.0f, texLeft, texBottom,
                1.0f, 1.0f, texRight, texBottom,
                1.0f, 0.0f, texRight, texTop,


                0.0f, 0.0f, texLeft, texTop,
                0.0f, 1.0f, texLeft, texBottom,
                1.0f, 1.0f, texRight, texBottom,
                1.0f, 0.0f, texRight, texTop,
            };

            if (!made){
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindVertexArray(VAO);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                made = true;

            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            bool noShader = Shader::GetCurrentShaderId() == 0;

            if (noShader){
                textureShader.Bind();
            }

            model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));

            model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
            model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

            model = glm::scale(model, glm::vec3(size.x * scale.x,size.y * scale.y, 1.0f));
            Point scr = ScreenManager::GetInstance().GetGameSize();
            projection = glm::ortho(0.0f, (float)scr.x,  (float)scr.y, 0.0f, -1.0f, 1.0f);



            unsigned int transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "model");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

            transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "projection");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

            ShaderSetter<BearColor>::SetUniform(textureShader.GetCurrentShaderId(),"spriteColor",recolor);
            ShaderSetter<int>::SetUniform(textureShader.GetCurrentShaderId(),"image",0);




            /*unsigned int ow = glGetUniformLocation(Shader::GetCurrentShaderId(), "OwO");
            std::cout << ow << " : "<<Shader::GetCurrentShaderId()<<"\n";
            glUniform1f(ow,0.3f);*/



            glActiveTexture(GL_TEXTURE0);
            glBindTexture( GL_TEXTURE_2D, t->id );

            glBindVertexArray(VAO);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glBindVertexArray(0);

            if (noShader){
                textureShader.Unbind();
            }
            return true;
        #endif // GL2
    #else
        double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
        double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
        dimensions2.x = pos.x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
        dimensions2.y = pos.y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
        dimensions2.h = clipRect.h*scaleRatioH*scaleY;
        dimensions2.w = clipRect.w*scaleRatioW*scaleX;
        SDL_RenderCopyEx(BearEngine->GetRenderer(),textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip);
    #endif // RENDER_OPENGL
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
    #endif // RENDER_OPENGL
}

void RenderHelp::SetupShaders(){
    static bool made = false;
    if (!made){
        made = true;
        polygonShader.Compile(GL_VERTEX_SHADER,"quadvertex.glvs");
        polygonShader.Compile(GL_FRAGMENT_SHADER,"quadfrag.glfs");
        polygonShader.Link();
        textureShader.Compile(GL_VERTEX_SHADER,"sprvertex.glvs");
        textureShader.Compile(GL_FRAGMENT_SHADER,"sprfrag.glfs");
        textureShader.Link();
    }
}

void RenderHelp::DrawLineColor(Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a,float thicc){
    #ifdef RENDER_OPENGL
    static GLuint VAO;
    static GLuint VBO;
    static bool made = false;

    float dist = p1.GetDistance(p2);
    glm::vec2 size(dist,dist);
    glm::mat4 model(1.0f);
    glm::mat4 projection;

    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 0.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 1.0f,
    };

    if (!made){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }


    Point scale(1,1);

    model = glm::translate(model, glm::vec3(p1.x, p1.y, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    float angle = p2.getDirection(p1);
    model = glm::rotate(model, (float)Geometry::toDeg(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size.x * scale.x,size.y * scale.y, 1.0f));

    Point scr = ScreenManager::GetInstance().GetGameSize();
    projection = glm::ortho(0.0f, (float)scr.x,  (float)scr.y, 0.0f, -1.0f, 1.0f);



    unsigned int transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));



    transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"spriteColor",BearColor(r/255.0f,g/255.0f,b/255.0f,a/255.0f));

    glBindVertexArray(VAO);

    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    if (noShader){
        polygonShader.Unbind();
    }

    #endif // RENDER_OPENGL
}


void RenderHelp::DrawSquareColor(Rect box,uint8_t r,uint8_t g,uint8_t b,uint8_t a,bool outline,float angle){
    #ifdef RENDER_OPENGL
    static GLuint VAO;
    static GLuint VBO;
    static bool made = false;


    glm::vec2 size(box.w,box.h);
    glm::mat4 model(1.0f);
    glm::mat4 projection;


    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    if (!made){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }


    Point scale(1,1);

    model = glm::translate(model, glm::vec3(box.x, box.y, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size.x * scale.x,size.y * scale.y, 1.0f));

    Point scr = ScreenManager::GetInstance().GetGameSize();
    projection = glm::ortho(0.0f, (float)scr.x,  (float)scr.y, 0.0f, -1.0f, 1.0f);



    unsigned int transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"spriteColor",BearColor(r/255.0f,g/255.0f,b/255.0f,a/255.0f));

    glBindVertexArray(VAO);
    if (outline){
        glDrawArrays(GL_LINE_LOOP, 0, 8);
    }else{
        glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
    }
    glBindVertexArray(0);

    if (noShader){
        polygonShader.Unbind();
    }

    #endif // RENDER_OPENGL


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
    id = renderedTexture;
    RenderHelp::RenderTexture(this,pos,Rect(0,0,-1,-1),0,scale);
    /*
    #ifdef RENDER_OPENGL
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glEnable(GL_TEXTURE_2D);


        float quadWidth = size_w;
        float quadHeight = size_h;


    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glScalef(scale.x , scale.y, 1.0f);
        glTranslatef(
                (pos.x  + quadWidth / 2.f  ),
                (pos.y + quadHeight/ 2.f  ),
        0.f );
    //scale
    glBegin( GL_TRIANGLE_FAN );
        glTexCoord2f(  0.0f , 1.0f ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f(  1.0f , 1.0f ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f(  1.0f , 0.0f ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  0.0f , 0.0f ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, 0 );

    #endif // RENDER_OPENGL
    */
}

bool TargetTexture::Bind(){
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    Point gameCanvas = ScreenManager::GetInstance().GetGameSize();

    #ifdef RENDER_OPENGL
    /*glViewport(0, 0, size_w, size_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size_w, size_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
    glViewport(0,0,size_w, size_h);
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

bool TargetTexture::FreeTexture(){
    glDeleteBuffers(1,&vbo_fbo_vertices);
    glDeleteFramebuffers(1,&Framebuffer);
    glDeleteRenderbuffers(1, &depthrenderbuffer);
	glDeleteTextures(1, &renderedTexture);

    //glDeleteRenderbuffers(1, &renderedTexture);
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
    DebugHelper::DisplayGlError();

    Bind();
    glDisable(GL_BLEND);
    //RenderHelp::DrawSquareColor(Rect(0,0,wa,ha),0,0,0,0);
    glEnable(GL_BLEND);
    UnBind();

    return true;

}



