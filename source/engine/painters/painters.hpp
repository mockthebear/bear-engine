#pragma once

#include "../../settings/definitions.hpp"
#include "../../framework/geometry.hpp"
#include "../../framework/chainptr.hpp"


#include SDL_LIB_HEADER
#include "../libheader.hpp"


#include "../basetypes.hpp"
#include "../shadermanager.hpp"
#include "../../framework/vertex.hpp"
#include <memory>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class ScreenManager;
class Vertex;
class Painter;

class BasicRenderData{
     public:
        BasicRenderData():position(0.0f,0.0f),size(1.0f,1.0f),color(1.0f,1.0f,1.0f,1.0f),flip(0),m_scale(1.0f,1.0f),m_angle(0.0f){}

        void SetAngle(float p_angle){
            m_angle = p_angle;
        }


        void SetScale(Point p_scale){
            m_scale = p_scale;
        }

        Point& GetScale(){return m_scale;};
        float& GetAngle(){return m_angle;};

        Point position;
        Point size;

        BearColor color;
        uint8_t flip;

    protected:
        friend class Painter;

        Point m_scale;
        float m_angle;
};


class RenderData : public BasicRenderData{
    public:
        RenderData():BasicRenderData(),center(0.0f,0.0f),m_clip(0.0f,0.0f,0.0f,0.0f),m_forwardClip(0.0f,1.0f,0.0f,1.0f),VertexArray(0),VertexBuffer(0),ElementBuffer(0){};

        ~RenderData();


        void SetClip(Rect r, Point textureSize){
            m_clip = r;
            size.x = m_clip.w;
            size.y = m_clip.h;
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


        Rect& GetClip(){return m_clip;};

        Point center;

        void Bind();
    private:
        friend class Painter;

        Rect m_clip;
        Rect m_forwardClip;

        void UpdateVertex();

        GLuint VertexArray;
        GLuint VertexBuffer;
        GLuint ElementBuffer;

};

typedef std::shared_ptr<RenderData> RenderDataPtr;
typedef std::shared_ptr<BasicRenderData> BasicRenderDataPtr;


typedef chain_ptr<BearTexture> TexturePtr;

class Painter{
  public:
    static glm::mat4 CalculateModel(BasicRenderDataPtr );

    static void SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels = nullptr );

    static void SetViewport(Point screenNow,Point offset = Point(0.0f,0.0f));
    static void SetProjection(Point size,int flipScreen = SDL_FLIP_NONE);
    static void SetProjection(Rect rproj);
    static glm::mat4 Projection;


    static bool RenderTexture(BearTexture *ptr,RenderDataPtr data);
    static void DrawVertex(VertexArrayObjectPtr v,BasicRenderDataPtr r,int drawMode = GL_TRIANGLES);



    static bool RenderPointTexture(BearTexture *ptr,RenderDataPtr data);
    static bool DrawSprites(int id);


    static BearTexture * MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter);

     static bool CanSupport(PainterSupport sup);


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
    static uint32_t GetSharedBuffer(int id);
  private:
    friend class ScreenManager;


    static bool SetupEnvoriment(ScreenManager *sm);
    static int GetMaxTextureSize();


    static void SetupShaders();




    static bool m_shaderBuilt;

    static Shader textureShader;
    static Shader pointTextureShader;
    static Shader polygonShader;
    static VertexArrayObject m_vao;

    static uint32_t Buffers[4];
};
