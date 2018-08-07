#include "tileset.hpp"


TileSet::TileSet(){
    m_tileSize = Point(16.0f,16.0f);
    m_columns = m_rows = 1;
}

TileSet::TileSet(PointInt tileSize,Sprite tset):TileSet(){
    m_tileSize = tileSize;
    SetSprite(tset);

}

void TileSet::SetSprite(Sprite sp){
    tileset = sp;
    if (tileset.IsLoaded()){
        m_sheetSize = tileset.GetSize();

        m_columns = m_sheetSize.x/m_tileSize.x;
        m_rows = m_sheetSize.y/m_tileSize.y;
    }else{
        m_columns = 1;
        m_rows = 1;
    }
}

void TileSet::RenderTile(int index,Point pos,uint8_t rotate,SDL_RendererFlip flip){
    if (tileset.IsLoaded()){
        if (index <= m_columns*m_rows && index != -1){
            int cx = index%m_columns;
            int cy = index/m_columns;
            tileset.SetClip(m_tileSize.x*cx,m_tileSize.y*cy,m_tileSize.x,m_tileSize.y);
            tileset.SetFlip(flip);
            tileset.Render(pos,rotate * 90);
        }
    }
}

Point TileSet::GetTileSize(){
    return m_tileSize;
}


