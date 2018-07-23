#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        Framebuffer = 0;
        depthrenderbuffer = 0;
        m_renderData = std::make_shared<RenderData>();
    };
    bool Generate(int w,int h);
    bool Bind();
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){m_renderData->SetScale(p);};

    GLuint vbo_fbo_vertices;
    GLuint Framebuffer;
    GLuint depthrenderbuffer;
    private:

        static GLint lastbuffer;
        RenderDataPtr m_renderData;
};
