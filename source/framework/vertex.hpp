#pragma once
#include "../engine/libheader.hpp"
#include "../engine/basetypes.hpp"
#include "geometry.hpp"
#include <memory>
#include <iostream>
#include <vector>




/*
 void clear(){
            vertexData.clear();
            indexes.clear();
            indexCount = 0;
        }

        void AddVertexes(int size,float *f);
        void AddIndices(int size,uint32_t *f);

        int Generate(Rect r,float theta = 0.0f, bool onlyOnes = true, const BearColor colors[4] = BCWhite4);
        int GenerateLineLoop(Rect r,bool onlyOnes = true, const BearColor colors[4] = BCWhite4);
        int Generate(Circle r,int triangleAmount);


        int Generate(Point p1, Point p2, BearColor c1 = BearColor(),  BearColor c2 = BearColor());
        int AddVertice(Point p1, BearColor color = BearColor());
        int AddVertice(GameVertice vert);
        int RepeatLastIndex();
        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
        uint32_t indexCount;

        bool useIndexes;
*/

class BasicTransformations{
    public:
        BasicTransformations():translation(0.0f,0.0f),size(0.0f, 0.0f),flip(0),scale(1.0f,1.0f),clip(0.0f,0.0f,0.0f,0.0f),forwardClip(0.0f,1.0f,0.0f,1.0f),defaultColor(1.0, 1.0, 1.0, 1.0){};
        BasicTransformations(PointInt pos):translation(pos.x,pos.y),size(0.0f, 0.0f),flip(0),scale(1.0f,1.0f),clip(0.0f,0.0f,0.0f,0.0f),forwardClip(0.0f,1.0f,0.0f,1.0f),defaultColor(1.0, 1.0, 1.0, 1.0){};

        Point translation;
        Point size;
        uint8_t flip;
        Point scale;
        Rect clip;
        Rect forwardClip;
        BearColor defaultColor;

        Point& GetScale(){return scale;};


        virtual void SetScale(Point p_scale){
            scale = p_scale;
        }

        virtual void SetClip(Rect r, Point textureSize){
            clip = r;
            size.x = clip.w;
            size.y = clip.h;
            if (textureSize.x == 0 || textureSize.y == 0){
                forwardClip = Rect(0.0f,1.0f,0.0f,1.0f);
                return;
            }
            forwardClip.x = clip.x / textureSize.x;
            forwardClip.y =  ( clip.x + clip.w ) / textureSize.x;
            forwardClip.w = clip.y / textureSize.y;
            forwardClip.h = ( clip.y + clip.h ) / textureSize.y;
        }

};

class AdvancedTransformations : public BasicTransformations{
    public:

        AdvancedTransformations():BasicTransformations(),center(0.0f,0.0f),angle(0.0f){};
        AdvancedTransformations(PointInt pos):BasicTransformations(pos),center(0.0f,0.0f),angle(0.0f){};

         void SetAngle(float p_angle){
            angle = p_angle;
        }
        void SetPosition(PointInt pos){
            translation = pos;
        }

        void SetClip(Rect r, Point textureSize){
            clip = r;
            size.x = clip.w;
            size.y = clip.h;
            if (textureSize.x == 0 || textureSize.y == 0){
                forwardClip = Rect(0.0f,1.0f,0.0f,1.0f);
                return;
            }
            forwardClip.x = clip.x / textureSize.x;
            forwardClip.y =  ( clip.x + clip.w ) / textureSize.x;
            forwardClip.w = clip.y / textureSize.y;
            forwardClip.h = ( clip.y + clip.h ) / textureSize.y;
            center = Point(size.x * scale.x * 0.5f, size.y * scale.y * 0.5f);
        }

        Rect& GetClip(){return clip;};
        float& GetAngle(){return angle;};

        virtual void SetScale(Point p_scale){
            scale = p_scale;
            center = Point(size.x * scale.x * 0.5f, size.y * scale.y * 0.5f);
        }

        Point center;
        float angle;
};


class VertexArrayObject{
  public:

    VertexArrayObject():m_vertexBuffer(0),m_elementBuffer(0),m_indexCount(0),m_useElementBuffer(true){}
    ~VertexArrayObject();

    uint32_t GetIndexCount(){
        return m_indexes.size();
    }

    int AddRect(Rect& box, BearColor&& color = BearColor());
    //int AddRect(Rect&& box, BasicTransformations &&bt, BearColor&& color = BearColor());
    int AddRect(AdvancedTransformations &adt);
    int AddOutlineRect(AdvancedTransformations &adt);


    void AddIndices(int size,uint32_t *indices);
    void AddVertexes(int size,Vertex *vertices);


    void clear(){
        m_vertexData.clear();
        m_indexes.clear();
        m_indexCount = 0;
    }
    void Bind();
    void UnBind();

    uint32_t GetVertexesCount() const{
        return m_vertexData.size() ;
    }

    uint32_t GetDrawElementsCount() const{
        return m_indexCount;
    }

    bool SetupVertexes();

    std::vector<Vertex> m_vertexData;
    std::vector<uint32_t> m_indexes;
    bool HasElementBuffer(){
        return m_useElementBuffer;
    }
  private:
    void AquireBufferIds();
    bool SetVertexBuffer();
    bool SetElementBuffer();
    //Necessary to hold the object buffer
    uint32_t m_vertexBuffer;
    uint32_t m_elementBuffer;
    uint32_t m_indexCount;

    bool m_useElementBuffer;
};


typedef std::shared_ptr<VertexArrayObject> VertexArrayObjectPtr;
