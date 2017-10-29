#include "tileset.hpp"
#include "gamebase.hpp"
#include <stdio.h>


BlockTileSet::BlockTileSet(int tilew,int tileh,Sprite tset){
    SetTileWidth(tilew);
    SetTileHeight(tileh);
    tileset = tset;
    if (tileset.IsLoaded()){
        columns = tileset.GetWidth()/GetTileWidth();
        rows = tileset.GetHeight()/GetTileHeight();
    }
}


void BlockTileSet::Render(int index,int x,int y){
    if (tileset.IsLoaded()){
        if (index <= columns*rows && index != -1){
            int cx = index%columns;
            int cy = index/columns;
            tileset.SetClip(GetTileWidth()*cx,GetTileHeight()*cy,GetTileHeight(),GetTileWidth());
            tileset.Render(x,y);
        }
    }
}

void BlockTileSet::RawRender(int index,int x,int y){

}

void BlockTileSet::Render2(int index,int x,int y,float cxe,float cye){
    if (tileset.IsLoaded()){
        if (index <= columns*rows && index != -1){
            int cx = index%columns;
            int cy = index/columns;
            tileset.SetScaleX(cxe);
            tileset.SetScaleY(cye);
            tileset.SetClip(GetTileWidth()*cx,GetTileHeight()*cy,GetTileHeight(),GetTileWidth());
            tileset.Render(x,y);
            tileset.SetScaleX(1);
            tileset.SetScaleY(1);
        }
    }
}

SDL_Texture *BlockTileSet::GetTexture(){
    //todo: remove blocktileset
    return nullptr;

}


