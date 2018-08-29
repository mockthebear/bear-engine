#include "vertex.hpp"
#include "../engine/basetypes.hpp"
#include "../engine/painters/painters.hpp"
#include "debughelper.hpp"

void Vertex::AddVertexes(int size,float *f){
    for (int i=0;i<size;i++){
        vertexData.emplace_back(f[i]);
        indexCount++;
    }
}
void Vertex::AddIndices(int size,uint32_t *f){
    if (useIndexes){
        for (int i=0;i<size;i++){
            indexes.emplace_back(f[i]);
        }
    }
}

int Vertex::Generate(Rect r, bool onlyOnes){
    if (onlyOnes){
        r = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }else{
        r.PositionSized();
    }
    float vertices[] = {
        r.x  , r.y,
        r.x  , r.h,
        r.w  , r.h,
        r.w  , r.y,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[8]);
    if (useIndexes){
        uint32_t indices[] = {
            indexCount, indexCount+1, indexCount+3,
            indexCount+1, indexCount+2, indexCount+3,
        };
        indexCount += 4;
        indexes.insert(indexes.end(), &indices[0], &indices[6]);
    }
    return 8;
}

int Vertex::GenerateLineLoop(Rect r, bool onlyOnes){
    if (onlyOnes){
        r = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }else{
        r.PositionSized();
    }
    uint32_t vtcs = 0;
    vtcs += Generate(Point(r.x, r.y), Point(r.w, r.y));
    vtcs += Generate(Point(r.w, r.y), Point(r.w, r.h));
    vtcs += Generate(Point(r.w, r.h), Point(r.x, r.h));
    vtcs += Generate(Point(r.x, r.h), Point(r.x, r.y));
    return vtcs;
}

int Vertex::RepeatLastIndex(){
    if (useIndexes){
        uint32_t indices[] = {
            indexCount-1,
        };
        indexes.insert(indexes.end(), &indices[0], &indices[1]);
    }
    return 0;
}

int Vertex::AddVertice(Point p1){
    float vertices[] = {
        p1.x  , p1.y,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[2]);
    if (useIndexes){
        uint32_t indices[] = {
            indexCount,
        };
        indexCount += 1;
        indexes.insert(indexes.end(), &indices[0], &indices[1]);
    }
    return 1;
}

int Vertex::Generate(Point p1,Point p2){
    float vertices[] = {
        p1.x  , p1.y,
        p2.x  , p2.y,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[4]);
    if (useIndexes){
        uint32_t indices[] = {
            indexCount, indexCount+1,
        };
        indexCount += 2;
        indexes.insert(indexes.end(), &indices[0], &indices[2]);
    }
    return 2;
}

int Vertex::Generate(Circle r, int triangleAmount){

    vertexData.emplace_back(0.0f);
    vertexData.emplace_back(0.0f);

    if (useIndexes){
        indexes.emplace_back(indexCount);
        indexCount ++;
    }

    float angle;
    for (int i = 0; i <= triangleAmount; i++){
        angle = i * 2.0f * Geometry::PI() / (float)triangleAmount;
        vertexData.emplace_back(cos(angle));
        vertexData.emplace_back(sin(angle));
        if (useIndexes){
            indexes.emplace_back(indexCount);
            indexCount ++;
        }
    }
    return 8;
}

#if defined(SUPPORT_VERTEX_ARRAY)

VertexArrayObject::~VertexArrayObject(){
    if (m_vertexArray != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        glDeleteBuffers(1, &m_vertexBuffer);
        if (m_elementBuffer != 0)
            glDeleteBuffers(1, &m_elementBuffer);
        glDeleteVertexArrays(1, &m_vertexArray);
        m_vertexArray = 0;
    }
}

void VertexArrayObject::Bind(){
    if (m_vertexArray != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        glBindVertexArray(m_vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        if (m_useElementBuffer)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    }
}

void VertexArrayObject::UnBind(){
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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


        generatedBuffers = true;
    }
    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,  vertexes.vertexData.size() * sizeof(float), &vertexes.vertexData[0], GL_DYNAMIC_DRAW);
    if (m_useElementBuffer){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  vertexes.indexes.size() * sizeof(uint32_t), &vertexes.indexes[0], GL_STATIC_DRAW);
    }



    if (generatedBuffers && manageBuffers){
        SetAttributes();
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_useElementBuffer)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void VertexArrayObject::SetAttributes(){
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
}

#else




VertexArrayObject::~VertexArrayObject(){
    #ifndef SUPPORT_SINGLE_BUFFER
    if (m_vertexBuffer != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        glDeleteBuffers(1, &m_vertexBuffer);
        if (m_elementBuffer != 0)
            glDeleteBuffers(1, &m_elementBuffer);
    }
    #endif // SUPPORT_SINGLE_BUFFER
}

void VertexArrayObject::Bind(){
    #ifdef SUPPORT_SINGLE_BUFFER
    m_vertexBuffer = Painter::GetSharedBuffer(0);
    m_elementBuffer = Painter::GetSharedBuffer(1);
    #endif
    if (m_vertexBuffer != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        #ifdef REMAKE_VETEX_ON_BIND
        SetupVertexes(m_useElementBuffer);
        #else
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        if (m_useElementBuffer)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        SetAttributes();
        #endif // REMAKE_VETEX_ON_BIND
    }
}

void VertexArrayObject::SetAttributes(){
    GLint posAttrib = 0;

    #ifdef NEED_SHADER_LOCATION
    uint32_t shaderId = 0;
    if ((shaderId = Shader::GetCurrentShaderId()) == 0){
        shaderId = Painter::polygonShader.GetId();
    }
    posAttrib = glGetAttribLocation(shaderId, "vPos");
    #endif // NEED_SHADER_LOCATION

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
}


void VertexArrayObject::UnBind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool VertexArrayObject::SetVertexBuffer(float * data,uint32_t size){
    #ifndef SUPPORT_SINGLE_BUFFER
    if (m_vertexBuffer == 0){
        glGenBuffers(1, &m_vertexBuffer);
        DisplayGlError("on gen buffers");
    }
    #else
    m_vertexBuffer = Painter::GetSharedBuffer(0);
    #endif

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    DisplayGlError("afterbind");
    if (m_vboSize != size){
        glBufferData(GL_ARRAY_BUFFER,  size, 0, GL_DYNAMIC_DRAW);
        m_vboSize = size;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    DisplayGlError("on set subdata");
    return true;
}

bool VertexArrayObject::SetElementBuffer(uint32_t * data,uint32_t size){
    if (!m_useElementBuffer){
        return false;
    }
    #ifndef SUPPORT_SINGLE_BUFFER
    if (m_elementBuffer == 0){
        glGenBuffers(1, &m_elementBuffer);
        DisplayGlError("on gen buffers");
    }
    #else
    m_elementBuffer = Painter::GetSharedBuffer(1);
    #endif
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    if (m_eboSize != size){
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  size, 0, GL_DYNAMIC_DRAW);
        m_eboSize = size;
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
    DisplayGlError("on set elems");
    return true;
}



bool VertexArrayObject::SetupVertexes(){
    if (!Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        return false;
    }
    DisplayGlError("pre bind");
    SetVertexBuffer(&vertexes.vertexData[0], vertexes.vertexData.size() * sizeof(float));
    SetElementBuffer(&vertexes.indexes[0], vertexes.indexes.size() * sizeof(uint32_t));
    SetAttributes();
    DisplayGlError("on set attr");
    return true;
}
/*
#else

VertexArrayObject::~VertexArrayObject(){};


bool VertexArrayObject::SetupVertexes(bool){
    return false;
}*/
#endif
