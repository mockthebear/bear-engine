#include "tileset.hpp"
#include "gamebase.hpp"
#include <stdio.h>

TileSet::TileSet(){
    m_tileWidth = m_tileHeight = 16;
    m_columns = m_rows = 1;
}

TileSet::TileSet(int tilew,int tileh,Sprite tset):TileSet(){

    m_tileWidth = tilew;
    m_tileHeight = tileh;
    SetSprite(tset);

}

void TileSet::SetSprite(Sprite &sp){

    tileset = sp;

    if (tileset.IsLoaded()){

        m_columns = tileset.GetWidth()/m_tileWidth;
        m_rows = tileset.GetHeight()/m_tileHeight;

    }else{

        m_columns = 1;
        m_rows = 1;

    }
}

void TileSet::Render(int index,Point pos){

    if (tileset.IsLoaded()){
        if (index <= m_columns*m_rows && index != -1){
            int cx = index%m_columns;
            int cy = index/m_columns;
            tileset.SetClip(m_tileWidth*cx,m_tileHeight*cy,m_tileWidth,m_tileHeight);
            tileset.Render(pos);
        }
    }

}



void TileSet::RenderScaled(int index,Point pos,float cxe,float cye){
    if (tileset.IsLoaded()){
        if (index <= m_columns*m_rows && index != -1){
            int cx = index%m_columns;
            int cy = index/m_columns;
            tileset.SetScaleX(cxe);
            tileset.SetScaleY(cye);
            tileset.SetClip(m_tileWidth*cx,m_tileHeight*cy,m_tileWidth,m_tileHeight);
            tileset.Render(pos);
            tileset.SetScaleX(1);
            tileset.SetScaleY(1);
        }
    }
}

Point TileSet::GetTileSize(){

    return Point(m_tileWidth,m_tileHeight);

}


