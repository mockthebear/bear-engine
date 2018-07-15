#pragma once

#include "../../settings/definitions.hpp"
#include "../../framework/geometry.hpp"
#include "../../framework/chainptr.hpp"
//Used to manage all the sort of rendering stuff

#include SDL_LIB_HEADER


#include "../basetypes.hpp"
#include "../shadermanager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class ScreenManager;


class RenderData{
    public:
        RenderData():position(0.0f,0.0f),clip(0.0f,0.0f,0.0f,0.0f),
                    angle(0.0f),center(0.0f,0.0f),scale(1.0f,1.0f),color{1.0f,1.0f,1.0f,1.0f},
                    flip(0),forwardClip(0.0f,1.0f,0.0f,1.0f),VBO(0),VAO(0),model(){};

    Point position;
    Rect clip;
    float angle;
    Point center;
    Point scale;
    float color[4];
    uint8_t flip;
    Rect forwardClip;
    GLuint VBO,VAO;

    glm::mat4 model;


    void SetClip(Rect r, Point textureSize){
        clip = r;
        if (textureSize.x == 0 || textureSize.y == 0){
            forwardClip = Rect(0.0f,1.0f,0.0f,1.0f);
            return;
        }
        forwardClip.x = clip.x / textureSize.x;
        forwardClip.y =  ( clip.x + clip.w ) / textureSize.x;
        forwardClip.w = clip.y / textureSize.y;
        forwardClip.h = ( clip.y + clip.h ) / textureSize.y;
        UpdateVertex();
    }

    void UpdateVertex();
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

        static void DrawSquare(Rect box,int r,int g,int b,int a);
    private:
        friend class ScreenManager;
        static bool SetupEnvoriment(ScreenManager *sm);
        static int GetMaxTextureSize();
        static void ResetViewPort(PointInt originalSize, PointInt newSize);

        static void SetupShaders();

        static Shader textureShader;
        static Shader polygonShader;


        static bool m_shaderBuilt;

};
