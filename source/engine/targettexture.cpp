#include "targettexture.hpp"
#include "screenmanager.hpp"
#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
/**
    TARGET TEXTURE
**/


void TargetTexture::Render(Point pos){
    m_renderData->position = pos;
    //
    Painter::RenderTexture(this,m_renderData);
}

bool TargetTexture::Bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    Point sz = ScreenManager::GetInstance().GetGameSize();
    ScreenManager::GetInstance().ForceProjection(sz,SDL_FLIP_VERTICAL);
    return true;
}


bool TargetTexture::UnBind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ScreenManager::GetInstance().ResetProjection();
    return true;
}

bool TargetTexture::FreeTexture(){
    uint32_t tex = DropTexture();
    if (tex > 0){
        glDeleteTextures(1, &tex);
        glDeleteFramebuffers(1, &m_frameBuffer);
        glDeleteRenderbuffers(1, &m_renderBuffer);
    }
    return true;
}

bool TargetTexture::Generate(int wa,int ha){

    glGenFramebuffers(1, &m_frameBuffer);
    TextureLoadMethod mthd(TEXTURE_NEAREST);

    texture_w = size_w = wa;
    texture_h = size_h = ha;

    BearTexture *tex = Painter::MakeTexture(PointInt(wa,ha), GL_RGBA, nullptr, mthd);
    id = tex->id;
    tex->DropTexture();
    delete tex;

    m_renderData->SetClip(Rect(0,0,size_w,size_h),Point(size_w,size_h));

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);



    glGenRenderbuffers(1, &m_renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, wa, ha);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        bear::out << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;

}

