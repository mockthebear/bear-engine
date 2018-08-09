#include "buffertilemap.hpp"
#include "../bear.hpp"
#ifndef SUPPORT_VERTEX_BUFFER
#include "../text.hpp"
#endif // SUPPORT_VERTEX_BUFFER

Shader BufferTileMap::m_shader;
bool BufferTileMap::m_madeShaders = false;


void BufferTileVAO::SetAttributes(){
    int posAttrib = 0;
    int clipAttrib = 1;
    int transLAttrib = 2;
    #ifdef NEED_SHADER_LOCATION

    uint32_t shaderId = BufferTileMap::m_shader.GetId();

    posAttrib = glGetAttribLocation(shaderId, "vPos");
    clipAttrib = glGetAttribLocation(shaderId, "clip");
    transLAttrib = glGetAttribLocation(shaderId, "translate");
    DebugHelper::DisplayGlError("on get attrs");
    #endif // NEED_SHADER_LOCATION

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,    2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)0);
    DebugHelper::DisplayGlError("on set attr 1");
    glEnableVertexAttribArray(clipAttrib);
    glVertexAttribPointer(clipAttrib,   2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
    DebugHelper::DisplayGlError("on set attr 2");
    glEnableVertexAttribArray(transLAttrib);
    glVertexAttribPointer(transLAttrib, 2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)) );
    DebugHelper::DisplayGlError("on set attr 3");
}


void BufferTileMap::checkShaders(){
    if (!m_madeShaders){
        m_shader.Compile(GL_VERTEX_SHADER, "data/shaders/ft_vertex.glfs");
        m_shader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultTextureFragmentShader);
        m_shader.Link();
        m_madeShaders = true;
    }
}

BufferTileMap::BufferTileMap(PointInt tileSize,PointInt3 mapSize, Sprite set):TileMap(tileSize,mapSize,set){
    #ifdef SUPPORT_VERTEX_BUFFER
    checkShaders();
    elemCount = 0;
    m_color = BearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_scale = Point(1.0f, 1.0f);
    UpdateModel();
    #endif // SUPPORT_VERTEX_BUFFER
}

void BufferTileMap::BindBuffer(){
    m_vao.Bind();
    DebugHelper::DisplayGlError("on bind");
}


float BufferTileMap::forwardSquares[4][8] = {
    {
        0.0f  , 0.0f,
        0.0f  , 1.0f,
        1.0f  , 1.0f,
        1.0f  , 0.0f,
    },
    {
        1.0f  , 0.0f,
        0.0f  , 0.0f,
        0.0f  , 1.0f,
        1.0f  , 1.0f,
    },
    {
        1.0f  , 1.0f,
        1.0f  , 0.0f,
        0.0f  , 0.0f,
        0.0f  , 1.0f,
    },
    {
        0.0f  , 1.0f,
        1.0f  , 1.0f,
        1.0f  , 0.0f,
        0.0f  , 0.0f,
    },
};

void BufferTileMap::InternalAddTile(uint32_t id,uint8_t rotate, SDL_RendererFlip flip, Point pos){
    if (id == m_blankTile){
        return;
    }
    Rect clip(
            (id % m_columns)        * m_tileSize.x,
            (int(id / m_columns))   * m_tileSize.y,
            m_tileSize.x,
            m_tileSize.y
    );
    Point textureSize = tileset.GetTextureSize();

    float texLeft =     clip.x / textureSize.x;
    float texRight =    ( clip.x + clip.w ) / textureSize.x;
    float texTop =      clip.y / textureSize.y;
    float texBottom =   ( clip.y + clip.h ) / textureSize.y;

    if ((flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }

    float * square = forwardSquares[rotate%4];

    float vertices[] = {
        // Pos      // Tex
        square[0]  , square[1]    , texLeft   , texTop,       pos.x,     pos.y,
        square[2]  , square[3]    , texLeft   , texBottom,    pos.x,     pos.y,
        square[4]  , square[5]    , texRight  , texBottom,    pos.x,     pos.y,
        square[6]  , square[7]    , texRight  , texTop,       pos.x,     pos.y,

    };

    unsigned int indices[] = {
        elemCount, elemCount+1, elemCount+3,
        elemCount+1, elemCount+2, elemCount+3,
    };
    elemCount += 4;

    m_vao.vertexes.indexes.insert(m_vao.vertexes.indexes.end(), &indices[0], &indices[6]);
    m_vao.vertexes.vertexData.insert(m_vao.vertexes.vertexData.end() , &vertices[0], &vertices[24]);
}

void BufferTileMap::UpdateBuffers(){
    #ifdef SUPPORT_VERTEX_BUFFER
    m_vao.clear();
    elemCount = 0;
    int l,y,x;
    for (l = 0; l < m_size.z; l++){
        for (y = 0; y < m_size.y; y++){
            for (x = 0; x < m_size.x; x++){
                Tile &t = m_map.get()[CalculateOffset(PointInt3(x,y,l))];
                if (t.id != m_blankTile){
                    InternalAddTile(t.id, t.rotate, t.flip, Point( x * m_tileSize.x, y * m_tileSize.y));
                }
            }
        }
    }
    m_vao.SetupVertexes();
    #endif // SUPPORT_VERTEX_BUFFER
}

void BufferTileMap::UpdateModel(){
    m_tileModel = glm::mat4(1.0f);
    m_tileModel[0][0] = m_tileModel[0][0] * (m_scale.x * m_tileSize.x);
    m_tileModel[1][0] = m_tileModel[1][0] * (m_scale.y * m_tileSize.y);
    m_tileModel[0][1] = m_tileModel[0][1] * (m_scale.x * m_tileSize.x);
    m_tileModel[1][1] = m_tileModel[1][1] * (m_scale.y * m_tileSize.y);
}

void BufferTileMap::Render(Point offset, Rect vision){
    #ifdef SUPPORT_VERTEX_BUFFER
    if (elemCount == 0){
        return;
    }


    m_shader.Bind();
    BindBuffer();




    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tileset.GetTexture().get()->id);
    DebugHelper::DisplayGlError("on texture");


    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Painter::Projection);
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",m_tileModel);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",m_color);
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"offset",offset);
    DebugHelper::DisplayGlError("on uniforms");


    glDrawElements(GL_TRIANGLES, m_vao.vertexes.indexes.size() , GL_UNSIGNED_INT, nullptr);
    DebugHelper::DisplayGlError("on rebder");
    glBindTexture( GL_TEXTURE_2D, 0 );


    m_shader.Unbind();
    #else
    Text T("[BufferTileMap]No support!",35);
    T.Render(offset);

    #endif

}


