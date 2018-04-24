#pragma once
#include "../framework/chainptr.hpp"
#include "../framework/chainptr.hpp"

#include SDL_LIB_HEADER
#ifdef RENDER_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include GLES_LIB
#endif // RENDER_OPENGLES

#ifdef RENDER_OPENGL
    #include GL_LIB
#endif // RENDER_OPENGLES

typedef class BearColor{
    public:
        BearColor():r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        BearColor(float f1):r(f1),g(f1),b(f1),a(f1){};
        BearColor(float f1,float f2,float f3,float f4):r(f1),g(f2),b(f3),a(f4){};
        float r;
        float g;
        float b;
        float a;
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
        BearTexture(){
            id = 0;
            w = h = c = 0;
            size_w = size_h = 0;
            textureMode = TEXTURE_NEAREST;
        };
        GLuint DropTexture(){
           GLuint ret = id;
           id = 0;
           return ret;
        }
        void ClearTexture(){
            GLuint tex = DropTexture();
            if (tex > 0)
                glDeleteTextures(1, &tex);
        }

        BearTexture(GLuint textureId,uint32_t width,uint32_t height,GLenum imgMode):id(textureId),w(width),h(height),mode(imgMode){};
        GLuint id;
        uint32_t w;
        uint32_t h;
        uint32_t c;
        uint32_t size_w;
        uint32_t size_h;
        TextureLoadMethod textureMode;
        GLenum mode;

};
typedef chain_ptr<BearTexture> TexturePtr;
