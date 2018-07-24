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
        }


        int Generate(Rect r);
        int Generate(Circle r,int triangleAmount);
        int Generate(Point p1, Point p2);
        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
};


class VertexArrayObject{
  public:

    int GetIndexCount(){
        return vertexes.indexes.size();
    }

    void clear(){
        vertexes.clear();
    }

    Vertex vertexes;

    ~VertexArrayObject();

    bool SetupVertexes();

    void Bind();

    uint32_t GetVertexArray(){
        return m_vertexArray;
    }
  private:
    int m_indexCount;

    uint32_t m_vertexArray;
    uint32_t m_vertexBuffer;
    uint32_t m_elementBuffer;
};


typedef std::shared_ptr<VertexArrayObject> VertexArrayObjectPtr;
