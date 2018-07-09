#include "targettexture.hpp"
#include "screenmanager.hpp"
#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
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
}

bool TargetTexture::Bind(){
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    Point gameCanvas = ScreenManager::GetInstance().GetGameSize();

    #ifdef RENDER_OPENGL
    glViewport(0, 0, size_w, size_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size_w, size_h, 0, -1, 1);
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
    RenderHelp::DrawSquareColor(Rect(0,0,wa,ha),0,0,0,0);
    glEnable(GL_BLEND);
    UnBind();

    return true;

}

