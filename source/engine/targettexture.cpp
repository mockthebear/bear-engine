#include "targettexture.hpp"
#include "screenmanager.hpp"
#include "renderhelp.hpp"
#include "../framework/debughelper.hpp"
/**
    TARGET TEXTURE
**/


void TargetTexture::Render(Point pos){
    m_renderData.SetPosition(pos);
    m_vertexes->AddRect(m_renderData);
    Painter::RenderTexture(this,m_vertexes);
    m_vertexes->clear();
}

bool TargetTexture::BindTexture(bool s_bind){
    if (id == 0){
        return false;
    }
    if (s_bind){
        glBindTexture(GL_TEXTURE_2D, id);
    }else{
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return true;
}

bool TargetTexture::BindRenderBuffer(bool s_bind){
    if (id == 0){
        return false;
    }
    if (s_bind){
        glBindRenderbuffer(GL_TEXTURE_2D, m_renderBuffer);
    }else{
        glBindRenderbuffer(GL_TEXTURE_2D, 0);
    }
    return true;
}
bool TargetTexture::Bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    Point originalScreen = ScreenManager::GetInstance().GetGameSize();
    Painter::SetViewport(originalScreen,Point(0.0, 0.0));
    Painter::SetProjection(Rect(0.0f, originalScreen.x, 0.0f, originalScreen.y));
    return true;
}
void TargetTexture::Clear(BearColor c){
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    RenderHelp::DrawSquareColor(RectColor(0 ,0 , size_w, size_w, c));
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
        m_frameBuffer = 0;
        m_renderBuffer = 0;
        tex = 0;
    }
    return true;
}

bool TargetTexture::Generate(int wa,int ha){
    if (m_frameBuffer != 0){
        FreeTexture();
    }

    texture_w = size_w = wa;
    texture_h = size_h = ha;

    BearTexture *tex = Painter::MakeTexture(PointInt(wa,ha), GL_RGBA, nullptr, textureMode);
    id = tex->id;
    tex->DropTexture();
    texture_w = tex->texture_w;
    texture_h = tex->texture_h;
    delete tex;
    m_renderData.SetClip(Rect(0,0,size_w,size_h),Point(texture_w,texture_h));

    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        bear::out << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_valid = true;

    return true;

}

