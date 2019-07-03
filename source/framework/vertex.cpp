#include "vertex.hpp"
#include "../engine/basetypes.hpp"
#include "../engine/painters/painters.hpp"
#include "debughelper.hpp"


void VertexArrayObject::AquireBufferIds(){
    #ifndef SUPPORT_SINGLE_BUFFER
    if (m_vertexBuffer == 0){
        glGenBuffers(1, &m_vertexBuffer);
        DisplayGlError("on gen vertex buffers");
        glGenBuffers(1, &m_elementBuffer);
        DisplayGlError("on gen element buffers");
    }
    #else
    m_vertexBuffer = Painter::GetSharedBuffer(0);
    m_elementBuffer = Painter::GetSharedBuffer(1);
    #endif
}



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
    AquireBufferIds();
    if (m_vertexBuffer != 0 && Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        #ifdef REMAKE_VETEX_ON_BIND
        //SetupVertexes(); necessary?
        #else
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        if (m_useElementBuffer)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        #endif // REMAKE_VETEX_ON_BIND
    }else{
        std::cout << "Failed to bind?\n";
    }
}


void VertexArrayObject::UnBind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool VertexArrayObject::SetVertexBuffer(){
    glBufferData(GL_ARRAY_BUFFER,  m_vertexData.size() * sizeof(Vertex), 0, GL_DYNAMIC_DRAW);
    DisplayGlError("now the buffer is located");

    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexData.size() * sizeof(Vertex), &m_vertexData[0]);
    DisplayGlError("on set subdata");

    return true;
}

bool VertexArrayObject::SetElementBuffer(){
    if (!m_useElementBuffer){
        return false;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  m_indexes.size() * sizeof(uint32_t), 0, GL_DYNAMIC_DRAW);
    DisplayGlError("now the buffer is located");

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexes.size() * sizeof(uint32_t), &m_indexes[0]);
    DisplayGlError("on set elems");
    return true;
}



bool VertexArrayObject::SetupVertexes(){
    if (!Painter::CanSupport(SUPPORT_VERTEXBUFFER)){
        return false;
    }
    DisplayGlError("pre bind");
    SetVertexBuffer();
    SetElementBuffer();
    DisplayGlError("on set attr");
    return true;
}


void VertexArrayObject::AddVertexes(int size,Vertex *vertices){
    m_vertexData.insert(m_vertexData.end(), &vertices[0], &vertices[size]);
}

void VertexArrayObject::AddIndices(int size,uint32_t *indices){
    m_indexCount += size;
    m_indexes.insert(m_indexes.end(), &indices[0], &indices[size]);
}


int VertexArrayObject::AddRect(Rect& box, BearColor&& color){

    Vertex vertices[4] = {
        Vertex(Point(box.x       , box.y), color),
        Vertex(Point(box.x       , box.y+box.h),  color),
        Vertex(Point(box.x+box.w , box.y+box.h),  color),
        Vertex(Point(box.x+box.w , box.y), color),
    };
    AddVertexes(4,vertices);

    if (m_useElementBuffer){
        uint32_t indices[] = {
            m_indexCount, m_indexCount+1, m_indexCount+3,
            m_indexCount+1, m_indexCount+2, m_indexCount+3,
        };
        m_indexCount += 6;
        AddIndices(6,indices);
    }
    return 6;
}


int VertexArrayObject::AddRect(AdvancedTransformations &adt){

    float texLeft = adt.forwardClip.x;
    float texRight =  adt.forwardClip.y;
    float texTop = adt.forwardClip.w;
    float texBottom = adt.forwardClip.h;




    if ((adt.flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((adt.flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }


    Vertex vertices[4] = {
        // Pos      // Tex
        Vertex(Point(0.0f                      , 0.0f                       ),  Point(texLeft   , texTop), adt.defaultColor),
        Vertex(Point(0.0f                      , adt.scale.y * adt.size.y   ),  Point(texLeft   , texBottom), adt.defaultColor),
        Vertex(Point(adt.scale.x * adt.size.x  , adt.scale.y * adt.size.y   ),  Point(texRight  , texBottom), adt.defaultColor),
        Vertex(Point(adt.scale.x * adt.size.x  , 0.0f                       ),  Point(texRight  , texTop), adt.defaultColor),

    };

    float auxa = Geometry::toRad(adt.angle);

    float s = sin(auxa);
    float c = cos(auxa);

    for (int i=0;i<4;i++){
        float px = vertices[i].x - adt.center.x;
        float py = vertices[i].y - adt.center.y;
        vertices[i].x = (px * c - py * s) + adt.center.x + adt.translation.x;
        vertices[i].y = (px * s + py * c) + adt.center.y + adt.translation.y;
    }

    AddVertexes(4,vertices);

    if (m_useElementBuffer){
        uint32_t indices[] = {
            m_indexCount, m_indexCount+1, m_indexCount+3,
            m_indexCount+1, m_indexCount+2, m_indexCount+3,
        };
        AddIndices(6,indices);
    }

    return 6;
}

int VertexArrayObject::AddOutlineRect(AdvancedTransformations &adt){

    float texLeft = adt.forwardClip.x;
    float texRight =  adt.forwardClip.y;
    float texTop = adt.forwardClip.w;
    float texBottom = adt.forwardClip.h;




    if ((adt.flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((adt.flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }


    Vertex vertices[4] = {
        // Pos      // Tex
        Vertex(Point(0.0f                      , 0.0f                       ),  Point(texLeft   , texTop), adt.defaultColor),
        Vertex(Point(0.0f                      , adt.scale.y * adt.size.y   ),  Point(texLeft   , texBottom), adt.defaultColor),
        Vertex(Point(adt.scale.x * adt.size.x  , adt.scale.y * adt.size.y   ),  Point(texRight  , texBottom), adt.defaultColor),
        Vertex(Point(adt.scale.x * adt.size.x  , 0.0f                       ),  Point(texRight  , texTop), adt.defaultColor),

    };

    float auxa = Geometry::toRad(adt.angle);

    float s = sin(auxa);
    float c = cos(auxa);

    for (int i=0;i<4;i++){
        float px = vertices[i].x - adt.center.x;
        float py = vertices[i].y - adt.center.y;
        vertices[i].x = (px * c - py * s) + adt.center.x + adt.translation.x;
        vertices[i].y = (px * s + py * c) + adt.center.y + adt.translation.y;
    }

    AddVertexes(4,vertices);

    if (m_useElementBuffer){
        uint32_t indices[] = {
            m_indexCount, m_indexCount+1,
            m_indexCount+1, m_indexCount+2,
            m_indexCount+2, m_indexCount+3,
            m_indexCount+3, m_indexCount,
        };
        AddIndices(8,indices);
    }

    return 6;
}


/*
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
}*/

