#pragma once
#include "libheader.hpp"

typedef class BearColor{
    public:
        BearColor():r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        BearColor(float f1):r(f1),g(f1),b(f1),a(f1){};
        BearColor(float f1,float f2,float f3,float f4):r(f1),g(f2),b(f3),a(f4){};
        BearColor(uint8_t f1,uint8_t f2,uint8_t f3,uint8_t f4):r(f1/255.0f),g(f2/255.0f),b(f3/255.0f),a(f4/255.0f){};
        float r,g,b,a;
        float& operator[](int x){
            switch (x){
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                    return a;
                default:
                    return a;
            }
        }
        float *Get4fv(){
            return &r;
        }
} BearColor;

enum TextureLoadMethodEnum{
    TEXTURE_DEFAULT,
    TEXTURE_NEAREST,
    TEXTURE_LINEAR,
    TEXTURE_TRILINEAR,
};


class TextureLoadMethod{
    public:
    static TextureLoadMethod DefaultLoadingMethod;
    TextureLoadMethod(){
        mode = TEXTURE_NEAREST;
    };
    TextureLoadMethod(TextureLoadMethodEnum md){
        mode = md;
    };
    void ApplyFilter(){
        switch (mode){
            case TEXTURE_NEAREST:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                break;
            case TEXTURE_LINEAR:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                break;
            case TEXTURE_TRILINEAR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
            case TEXTURE_DEFAULT:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                break;
        }
    }
    TextureLoadMethodEnum mode;
};




class BearTexture{
    public:
        BearTexture():id(0),texture_w(0),texture_h(0),size_w(0),size_h(0),textureMode(TEXTURE_NEAREST),mode(GL_RGBA){};

        uint32_t DropTexture();
        void ClearTexture();
        void ApplyPixels(uint8_t *pixels);

        BearTexture(uint32_t textureId,uint32_t imagewidth,uint32_t imageheight,uint32_t texturewidth,uint32_t textureheight,GLenum imgMode)
        :id(textureId),texture_w(texturewidth),texture_h(textureheight),size_w(imagewidth),size_h(imageheight),textureMode(TEXTURE_NEAREST),mode(imgMode){};
        uint32_t id;
        uint32_t texture_w;
        uint32_t texture_h;
        uint32_t size_w;
        uint32_t size_h;
        TextureLoadMethod textureMode;
        GLenum mode;

};

enum PainterSupport{
    SUPPORT_SHADERS = 0,
    SUPPORT_VERTEXBUFFER,
    SUPPORT_ORTOHOVERTEX,
    SUPPORT_POINTSPRITE,
};

