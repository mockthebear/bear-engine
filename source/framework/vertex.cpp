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
/*
int Vertex::Generate(Rect r, bool onlyOnes, float ang){
    if (onlyOnes){
        r = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }
    Point center(r.w/2.0f ,  r.h/2.0f);
    ang = Geometry::toRad(ang);
    float s = sin(ang);
    float c = cos(ang);
    float vertices[] = {
        0.0f    , 0.0f, 1.0, 1.0, 1.0, 1.0,
        0.0f    , r.h,  1.0, 1.0, 1.0, 1.0,
        r.w     , r.h,  1.0, 1.0, 1.0, 1.0,
        r.w     , 0.0f, 1.0, 1.0, 1.0, 1.0,
    };
    for (int i=0;i<4;i++){
        float px = vertices[i * 6  + 0] -= center.x;
        float py = vertices[i * 6  + 1] -= center.y;
        vertices[i * 6  + 0] = (px * c - py * s) + center.x + r.x;
        vertices[i * 6  + 1] = (px * s + py * c) + center.y + r.y;
    }
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[24]);

    if (useIndexes){
        uint32_t indices[] = {
            indexCount, indexCount+1, indexCount+3,
            indexCount+1, indexCount+2, indexCount+3,
        };
        indexCount += 4;
        indexes.insert(indexes.end(), &indices[0], &indices[6]);
    }
    return 4;
}*/



int Vertex::Generate(Rect r,float ang, bool onlyOnes, const BearColor colors[4]){
    if (onlyOnes){
        r = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }
    Point center(r.w/2.0f ,  r.h/2.0f);
    ang = Geometry::toRad(ang);
    float s = sin(ang);
    float c = cos(ang);
    float vertices[] = {
        0.0f    , 0.0f, colors[0].r, colors[0].g, colors[0].b, colors[0].a,
        0.0f    , r.h,  colors[1].r, colors[1].g, colors[1].b, colors[1].a,
        r.w     , r.h,  colors[2].r, colors[2].g, colors[2].b, colors[2].a,
        r.w     , 0.0f, colors[3].r, colors[3].g, colors[3].b, colors[3].a,
    };
    for (int i=0;i<4;i++){
        float px = vertices[i * 6  + 0] -= center.x;
        float py = vertices[i * 6  + 1] -= center.y;
        vertices[i * 6  + 0] = (px * c - py * s) + center.x + r.x;
        vertices[i * 6  + 1] = (px * s + py * c) + center.y + r.y;
    }
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[24]);

    if (useIndexes){
        uint32_t indices[] = {
            indexCount, indexCount+1, indexCount+3,
            indexCount+1, indexCount+2, indexCount+3,
        };
        indexCount += 4;
        indexes.insert(indexes.end(), &indices[0], &indices[6]);
    }
    return 4;
}

int Vertex::GenerateLineLoop(Rect r, bool onlyOnes, const BearColor colors[4]){
    if (onlyOnes){
        r = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    }else{
        r.PositionSized();
    }
    uint32_t vtcs = 0;
    vtcs += Generate(Point(r.x, r.y), Point(r.w, r.y), colors[0], colors[1]);
    vtcs += Generate(Point(r.w, r.y), Point(r.w, r.h), colors[1], colors[2]);
    vtcs += Generate(Point(r.w, r.h), Point(r.x, r.h), colors[2], colors[3]);
    vtcs += Generate(Point(r.x, r.h), Point(r.x, r.y), colors[3], colors[0]);
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

int Vertex::AddVertice(GameVertice vert){
    float vertices[] = {
        vert.x  , vert.y, vert.r, vert.g, vert.b, vert.a,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[6]);
    //vertexData.insert(vertexData.end(), vert.begin(), vert.end());
    if (useIndexes){
        uint32_t indices[] = {
            indexCount,
        };
        indexCount += 1;
        indexes.insert(indexes.end(), &indices[0], &indices[1]);
    }
    return 1;
}

int Vertex::AddVertice(Point p1, BearColor c){
    float vertices[] = {
        p1.x  , p1.y, c.r, c.g, c.b, c.a,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[6]);
    if (useIndexes){
        uint32_t indices[] = {
            indexCount,
        };
        indexCount += 1;
        indexes.insert(indexes.end(), &indices[0], &indices[1]);
    }
    return 1;
}

int Vertex::Generate(Point p1,Point p2, BearColor c1,  BearColor c2){
    float vertices[] = {
        p1.x  , p1.y, c1.r, c1.g, c1.b, c1.a,
        p2.x  , p2.y, c2.r, c2.g, c2.b, c2.a,
    };
    vertexData.insert(vertexData.end(), &vertices[0], &vertices[12]);
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


    //AddVertice(Point(r.x, r.y), BearColor());
    vertexData.emplace_back(r.x);
    vertexData.emplace_back(r.y);
    vertexData.emplace_back(1.0f);
    vertexData.emplace_back(1.0f);
    vertexData.emplace_back(1.0f);
    vertexData.emplace_back(1.0f);

    if (useIndexes){
        indexes.emplace_back(indexCount);
        indexCount ++;
    }


    float angle;
    for (int i = 0; i <= triangleAmount; i++){
        angle = i * 2.0f * Geometry::PI() / (float)triangleAmount;
        vertexData.emplace_back(r.x + r.r * cos(angle));
        vertexData.emplace_back(r.y + r.r * sin(angle));
        vertexData.emplace_back(1.0f);
        vertexData.emplace_back(1.0f);
        vertexData.emplace_back(1.0f);
        vertexData.emplace_back(1.0f);
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

bool VertexArrayObject::SetupVertexes(){
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



    if (generatedBuffers){
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
        SetupVertexes();
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
    GLint colorAttrib = 1;

    #ifdef NEED_SHADER_LOCATION
    uint32_t shaderId = 0;
    if ((shaderId = Shader::GetCurrentShaderId()) == 0){
        shaderId = Painter::polygonShader.GetId();
    }
    posAttrib = glGetAttribLocation(shaderId, "vPos");
    colorAttrib = glGetAttribLocation(shaderId, "vColor");
    #endif // NEED_SHADER_LOCATION

    /*glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), 0); //Position
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(2 * sizeof(float))); //Vertex clip
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(6 * sizeof(float))); //image size
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(8 * sizeof(float))); //scale
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(10 * sizeof(float))); //rotation
        */

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)) );

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
        DisplayGlError("now the buffer is located");
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
c
VertexArrayObject::~VertexArrayObject(){};


bool VertexArrayObject::SetupVertexes(bool){
    return false;
}*/
#endif
