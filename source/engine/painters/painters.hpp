#pragma once

#include "../../settings/definitions.hpp"
#include "../../framework/geometry.hpp"
#include "../../framework/chainptr.hpp"
//Used to manage all the sort of rendering stuff

#include SDL_LIB_HEADER


#include "../basetypes.hpp"
#include "../shadermanager.hpp"
#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class ScreenManager;
class Vertex;
class Painter;


class RenderData{
    public:
        RenderData():position(0.0f,0.0f),center(0.0f,0.0f),color{1.0f,1.0f,1.0f,1.0f},
        flip(0),model(),m_modelUpdateNeeded(true),m_scale(1.0f,1.0f),m_angle(0.0f),
        m_clip(0.0f,0.0f,0.0f,0.0f),m_forwardClip(0.0f,1.0f,0.0f,1.0f),TextureVertexArray(0),TextureVertexBuffer(0),TextureElementBuffer(0){};

        ~RenderData(){
            if (TextureVertexArray != 0){
                glDeleteBuffers(1, &TextureVertexBuffer);
                glDeleteBuffers(1, &TextureElementBuffer);
                glDeleteVertexArrays(1, &TextureVertexArray);
                TextureVertexArray = 0;
            }
        };

    Point position;

    Point center;
    float color[4];
    uint8_t flip;


    glm::mat4 model;

    void SetAngle(float p_angle){
        if (p_angle != m_angle){
            m_modelUpdateNeeded = true;
        }
        m_angle = p_angle;
    }


    void SetScale(Point p_scale){
        if (p_scale != m_scale){
           m_modelUpdateNeeded = true;
        }
        m_scale = p_scale;
    }

    void SetClip(Rect r, Point textureSize){
        if (r.w != m_clip.w || r.h != m_clip.h){
            m_modelUpdateNeeded = true;
        }
        m_clip = r;
        if (textureSize.x == 0 || textureSize.y == 0){
            m_forwardClip = Rect(0.0f,1.0f,0.0f,1.0f);
            return;
        }
        m_forwardClip.x = m_clip.x / textureSize.x;
        m_forwardClip.y =  ( m_clip.x + m_clip.w ) / textureSize.x;
        m_forwardClip.w = m_clip.y / textureSize.y;
        m_forwardClip.h = ( m_clip.y + m_clip.h ) / textureSize.y;
        UpdateVertex();
    }

    Point& GetScale(){return m_scale;};
    float& GetAngle(){return m_angle;};
    Rect& GetClip(){return m_clip;};

    private:
        friend class Painter;

        bool m_modelUpdateNeeded;
        Point m_scale;
        float m_angle;
        Rect m_clip;
        Rect m_forwardClip;

        void UpdateVertex();
        void UpdateModel();

        GLuint TextureVertexArray;
        GLuint TextureVertexBuffer;
        GLuint TextureElementBuffer;



};

typedef std::shared_ptr<RenderData> RenderDataPtr;


typedef chain_ptr<BearTexture> TexturePtr;

class Painter{
  public:
    static bool RenderTexture(BearTexture *ptr,RenderDataPtr data);
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
        static void DrawVertex(Vertex &v,BearColor c,int drawMode = GL_TRIANGLES);


        static void DrawSquare(Rect box,BearColor c,bool outline=false,float angle=0);
        static void DrawLine(Point p1,Point p2,BearColor c,float thicc);




    private:
        friend class ScreenManager;
        static bool SetupEnvoriment(ScreenManager *sm);
        static int GetMaxTextureSize();
        static void ResetViewPort(PointInt originalSize, PointInt newSize);

        static void SetupShaders();

        static Shader textureShader;
        static Shader polygonShader;


        static bool m_shaderBuilt;

        //

        static void SetupPolygonVAOs();

        static GLuint SharedVertexArray;
        static GLuint SharedVertexBuffer;




};
