#include "rendertexture.hpp"



void RenderTexture::RenderAll(){
    Painter::RenderTexture(m_texture.get(),m_vertexes);
}


Point RenderTexture::GetCenter(){
    return m_renderData.center;
}


void RenderTexture::SetCenter(Point p){
    m_renderData.center.x = p.x;
    m_renderData.center.y = p.y;
};


void RenderTexture::ReBlend(uint8_t Red,uint8_t Blue,uint8_t Green){
    m_renderData.defaultColor[0] = Red/255.0f;
    m_renderData.defaultColor[1] = Blue/255.0f;
    m_renderData.defaultColor[2] = Green/255.0f;

};

void RenderTexture::SetAlpha(uint8_t alpha){
    m_renderData.defaultColor[3] = alpha/255.0f;
};

uint8_t RenderTexture::GetAlpha(){
    return m_renderData.defaultColor[3]*255;
};


bool RenderTexture::IsLoaded(){
    return m_texture.get() != nullptr ;
};

TexturePtr RenderTexture::GetTexture(){
    return m_texture;
};


bool RenderTexture::HasTexture(){
    return m_texture.get() != nullptr;
}

void RenderTexture::SetTexture(TexturePtr t){
    m_texture = t;
};

AdvancedTransformations& RenderTexture::GetRenderData(){
    return m_renderData;
}

void RenderTexture::SetFlip(SDL_RendererFlip flipState){
    m_renderData.flip = (uint8_t)flipState;
}

SDL_RendererFlip RenderTexture::GetFlip(){
    return (SDL_RendererFlip)m_renderData.flip;
}


void RenderTexture::SetScaleX(float scale){
    m_renderData.GetScale().x=scale;
};

void RenderTexture::SetScaleY(float scale){
    m_renderData.GetScale().y=scale;
};

void RenderTexture::SetScale(Point t_scale){
    m_renderData.SetScale(t_scale);
};

Point RenderTexture::GetScale(){
    return m_renderData.GetScale();
};
