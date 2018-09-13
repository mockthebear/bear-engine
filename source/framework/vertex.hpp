#pragma once
#include "../engine/libheader.hpp"
#include "geometry.hpp"
#include <memory>
#include <iostream>
#include <vector>



class Vertex{
    public:
        Vertex(bool useInd = true):useIndexes(useInd){};

        void clear(){
            vertexData.clear();
            indexes.clear();
            indexCount = 0;
        }

        void AddVertexes(int size,float *f);
        void AddIndices(int size,uint32_t *f);


        int Generate(Rect r,bool onlyOnes = true, float theta = 0);
        int GenerateLineLoop(Rect r,bool onlyOnes = true);
        int Generate(Circle r,int triangleAmount);
        int Generate(Point p1, Point p2);
        int AddVertice(Point p1);
        int RepeatLastIndex();
        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
        uint32_t indexCount;

        bool useIndexes;
};


class VertexArrayObject{
  public:

    VertexArrayObject(bool useEbo = true):vertexes(useEbo),m_useElementBuffer(useEbo),m_vertexArray(0),m_vertexBuffer(0),m_elementBuffer(0),m_vboSize(0),m_eboSize(0){}
    uint32_t GetIndexCount(){
        return vertexes.indexes.size();
    }

    void clear(){
        vertexes.clear();

    }

    uint32_t GetVertexesCount() const{
        return m_vboSize / sizeof(float);
    }

    Vertex vertexes;

    ~VertexArrayObject();

    bool SetVertexBuffer(float * data,uint32_t size);
    bool SetElementBuffer(uint32_t * data,uint32_t size);

    bool SetupVertexes();

    void Bind();
    static void UnBind();

    void DisableElements(){
        m_useElementBuffer = false;
    }

    bool HasElementBuffer() const{
        return m_useElementBuffer;
    }

    uint32_t GetVertexArray() const{
        return m_vertexArray;
    }
    uint32_t GetVertexBuffer() const{
        return m_vertexBuffer;
    }

    uint32_t GetElementBuffer() const{
        return m_elementBuffer;
    }

    virtual void SetAttributes();

    static uint32_t VBOMASTER;
    static uint32_t VBOSIZE;
    static uint32_t ELEMMASTER;

  private:
    bool m_useElementBuffer;


    uint32_t m_vertexArray;
    uint32_t m_vertexBuffer;
    uint32_t m_elementBuffer;
    uint32_t m_vboSize;
    uint32_t m_eboSize;
};


typedef std::shared_ptr<VertexArrayObject> VertexArrayObjectPtr;
