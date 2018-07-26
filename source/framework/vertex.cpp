#include "vertex.hpp"
#include "../engine/basetypes.hpp"
#include "../engine/painters/painters.hpp"

void Vertex::AddVertexes(int size,float *f){
    for (int i=0;i<size;i++){
        vertexData.emplace_back(f[i]);
    }
}
void Vertex::AddIndices(int size,uint32_t *f){
    for (int i=0;i<size;i++){
        indexes.emplace_back(f[i]);
    }
}

int Vertex::Generate(Rect r){
    float vertices[] = {
        0.0f  , 0.0f,
        0.0f  , 1.0f,
        1.0f  , 1.0f,
        1.0f  , 0.0f,
    };
    uint32_t indices[] = {
        0, 1, 2, 3,
    };
    vertexData.insert(vertexData.begin(), &vertices[0], &vertices[8]);
    indexes.insert(indexes.begin(), &indices[0], &indices[4]);
    return 8;
}


int Vertex::Generate(Point,Point){
    float vertices[] = {
        0.0f  , 0.0f,
        1.0f  , 1.0f,
    };
    uint32_t indices[] = {
        0, 1,
    };
    vertexData.insert(vertexData.begin(), &vertices[0], &vertices[4]);
    indexes.insert(indexes.begin(), &indices[0], &indices[2]);
    return 2;
}

int Vertex::Generate(Circle r, int triangleAmount){

    vertexData.emplace_back(0.0f);
    vertexData.emplace_back(0.0f);

    indexes.emplace_back(0);

    float angle;
    for (int i = 0; i <= triangleAmount; i++){
        angle = i * 2.0f * Geometry::PI() / (float)triangleAmount;
        vertexData.emplace_back(cos(angle));
        vertexData.emplace_back(sin(angle));
        indexes.emplace_back(i+1);
    }


    /*float vertices[] = {
        0.0f  , 0.0f,
        0.0f  , 1.0f,
        1.0f  , 1.0f,
        1.0f  , 0.0f,
    };
    uint32_t indices[] = {
        0, 1, 2, 3,
    };*/

    return 8;
}

#if defined(RENDER_OPENGL3) || defined(RENDER_OPENGL)

VertexArrayObject::~VertexArrayObject(){
    if (m_vertexArray != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        glDeleteBuffers(1, &m_vertexBuffer);
        glDeleteBuffers(1, &m_elementBuffer);
        glDeleteVertexArrays(1, &m_vertexArray);
        m_vertexArray = 0;
    }
}

void VertexArrayObject::Bind(){
    if (m_vertexArray != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        glBindVertexArray(m_vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    }
}

void VertexArrayObject::UnBind(){
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool VertexArrayObject::SetupVertexes(bool manageBuffers){
    if (!Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        return false;
    }
    bool generatedBuffers = false;
    if (m_vertexArray == 0){
        glGenVertexArrays(1, &m_vertexArray);
        glGenBuffers(1, &m_vertexBuffer);
        if (m_useElementBuffer)
            glGenBuffers(1, &m_elementBuffer);

        glBindVertexArray(m_vertexArray);
        generatedBuffers = true;
    }


    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,  vertexes.vertexData.size() * sizeof(float), &vertexes.vertexData[0], GL_DYNAMIC_DRAW);
    if (m_useElementBuffer){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  vertexes.indexes.size() * sizeof(uint32_t), &vertexes.indexes[0], GL_STATIC_DRAW);
    }



    if (generatedBuffers && manageBuffers){
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_useElementBuffer)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

#else

VertexArrayObject::~VertexArrayObject(){};


bool VertexArrayObject::SetupVertexes(){
    return false;
}
#endif
