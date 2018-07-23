#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        Framebuffer = 0;
        depthrenderbuffer = 0;
    };
    bool Generate(int w,int h);
    bool Bind();
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){m_renderData.m_scale = (p);};

    GLuint vbo_fbo_vertices;
    GLuint Framebuffer;
    GLuint depthrenderbuffer;
    private:

        static GLint lastbuffer;
        RenderData m_renderData;
};
