#pragma once
#include "../engine/libheader.hpp"
#include "geometry.hpp"
#include <memory>
#include <iostream>
#include <vector>



class Vertex{
    public:
        Vertex(){};

        void clear(){
            vertexData.clear();
            indexes.clear();
            indexCount = 0;
        }

        void AddVertexes(int size,float *f);
        void AddIndices(int size,uint32_t *f);


        int Generate(Rect r,bool onlyOnes = true);
        int GenerateLineLoop(Rect r,bool onlyOnes = true);
        int Generate(Circle r,int triangleAmount);
        int Generate(Point p1, Point p2);
        int AddVertice(Point p1);
        int RepeatLastIndex();
        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
        uint32_t indexCount;
};


class VertexArrayObject{
  public:

    VertexArrayObject():m_useElementBuffer(true),m_vertexArray(0),m_vertexBuffer(0),m_elementBuffer(0){}
    uint32_t GetIndexCount(){
        return vertexes.indexes.size();
    }

    void clear(){
        vertexes.clear();

    }

    uint32_t GetVertexesCount(){
        return vertexes.vertexData.size();
    }

    Vertex vertexes;

    ~VertexArrayObject();

    bool SetupVertexes(bool manageBuffers = true);

    void Bind();
    static void UnBind();

    void DisableElements(){
        m_useElementBuffer = false;
    }

    uint32_t GetVertexArray(){
        return m_vertexArray;
    }
    uint32_t GetVertexBuffer(){
        return m_vertexBuffer;
    }

    uint32_t GetElementBuffer(){
        return m_elementBuffer;
    }

    virtual void SetAttributes();

    static uint32_t VBOMASTER;
    static uint32_t ELEMMASTER;

  private:
    bool m_useElementBuffer;

    uint32_t m_vertexArray;
    uint32_t m_vertexBuffer;
    uint32_t m_elementBuffer;
};


typedef std::shared_ptr<VertexArrayObject> VertexArrayObjectPtr;
