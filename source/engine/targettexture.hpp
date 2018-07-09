#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        Framebuffer = 0;
        renderedTexture = 0;
        depthrenderbuffer = 0;
        scale = Point(1.0f,1.0f);
    };
    bool Generate(int w,int h);
    bool Bind();
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){scale = p;};

    GLuint vbo_fbo_vertices;
    GLuint renderedTexture;
    GLuint Framebuffer;
    GLuint depthrenderbuffer;
    static GLint lastbuffer;
    Point scale;
};
