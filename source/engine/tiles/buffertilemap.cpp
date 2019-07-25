#include "buffertilemap.hpp"
#include "../bear.hpp"
#ifndef SUPPORT_VERTEX_BUFFER
#include "../text.hpp"
#endif // SUPPORT_VERTEX_BUFFER

BufferTileMap::BufferTileMap(PointInt tileSize,PointInt3 mapSize, Sprite set):TileMap(tileSize,mapSize,std::move(set) ){
    m_vertexes = std::make_shared<VertexArrayObject>();
}

/*
float BufferTileMap::forwardIndices[4][8] = {
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
};*/

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


    Vertex vertices[4] = {
        // Pos      // Tex
        Vertex(Point(pos.x                  , pos.y                      ),  Point(texLeft   , texTop)),
        Vertex(Point(pos.x                  , pos.y + m_tileSize.y       ),  Point(texLeft   , texBottom)),
        Vertex(Point(pos.x + m_tileSize.x   , pos.y + m_tileSize.y   ),  Point(texRight  , texBottom)),
        Vertex(Point(pos.x + m_tileSize.x   , pos.y                  ),  Point(texRight  , texTop)),

    };


    m_vertexes->AddRectIndexes(false);
    m_vertexes->AddVertexes(4,vertices);

}

void BufferTileMap::UpdateBuffers(){
    #ifdef SUPPORT_VERTEX_BUFFER
    m_vertexes->clear();
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
    #endif // SUPPORT_VERTEX_BUFFER
}

void BufferTileMap::Render(Point offset, Rect vision){
    #ifdef SUPPORT_VERTEX_BUFFER
    Painter::textureShader.Bind();
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"g_offset",offset);
    Painter::RenderTexture(tileset.GetTexture().get(),m_vertexes);
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"g_offset",Point(0.0f,0.0f));
    Painter::textureShader.Unbind();
    #else
    Text T("[BufferTileMap]No support!",35);
    T.Render(offset);

    #endif

}


