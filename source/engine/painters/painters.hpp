#pragma once

#include "../../settings/definitions.hpp"
#include "../../framework/geometry.hpp"
#include "../../framework/chainptr.hpp"
//Used to manage all the sort of rendering stuff

#include SDL_LIB_HEADER
#ifdef RENDER_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include GLES_LIB
#endif // RENDER_OPENGLES

#ifdef RENDER_OPENGL
    #include GL_LIB
#endif // RENDER_OPENGLES

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
            texture_w = texture_h = 0;
            size_w = size_h = 0;
            textureMode = TEXTURE_NEAREST;
        };
        uint32_t DropTexture(){
           uint32_t ret = id;
           id = 0;
           return ret;
        }
        void ClearTexture(){
            uint32_t tex = DropTexture();
            if (tex > 0)
                glDeleteTextures(1, &tex);
        }

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

class RenderData{
    public:
        RenderData():position(0.0f,0.0f),clip(0.0f,0.0f,0.0f,0.0f),
                    angle(0.0f),center(0.0f,0.0f),scale(1.0f,1.0f),color{1.0f,1.0f,1.0f,1.0f},
                    flip(0){};

    Point position;
    Rect clip;
    float angle;
    Point center;
    Point scale;
    float color[4];
    uint8_t flip;
};


typedef chain_ptr<BearTexture> TexturePtr;

class Painter{
  public:
    static bool RenderTexture(BearTexture *ptr,RenderData &data);
    static BearTexture * MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter);

    static uint32_t powerOfTwo( uint32_t num ){
        if( num != 0 ){
            if ((num & (num - 1)) == 0){
                return num;
            }
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
};
