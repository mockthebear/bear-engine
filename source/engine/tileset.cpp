#include "tileset.hpp"
#include <stdio.h>
MultiTileSet::MultiTileSet(int tileWidth2,int tileHeight2){
    SetTileHeight(tileHeight2);
    SetTileWidth(tileWidth2);

}


MultiTileSet::~MultiTileSet(){
    for(int i = tileArray.size() - 1; i >= 0; --i)
    {
        delete tileArray[i];
    }

}

void MultiTileSet::Open(const char* path){
    Sprite *s = new Sprite(path);
    tileArray.emplace_back(s);
}

void MultiTileSet::Render(int ind,int x,int y){
    if (ind > -1)
        tileArray[ind]->Render(x,y);
}


BlockTileSet::BlockTileSet(int tilew,int tileh,char *file){
    SetTileWidth(tilew);
    tileset = NULL;
    SetTileHeight(tileh);
    Open(file);
}

void BlockTileSet::Open(const char *file){
    tileset = new Sprite(file);
    if (tileset != NULL){
        columns = tileset->GetWidth()/GetTileWidth();
        rows = tileset->GetHeight()/GetTileHeight();
    }
}

BlockTileSet::~BlockTileSet(){
    if (tileset)
        delete tileset;
}

void BlockTileSet::Render(int index,int x,int y){
    if (tileset != NULL){
        if (index <= columns*rows and index != -1){
            int cx = index%columns;
            int cy = index/columns;
            tileset->SetClip(GetTileWidth()*cx,GetTileHeight()*cy,GetTileHeight(),GetTileWidth());
            tileset->Render(x,y);
        }
    }
}

void BlockTileSet::Render2(int index,int x,int y,float cxe,float cye){
    if (tileset != NULL){
        if (index <= columns*rows and index != -1){
            int cx = index%columns;
            int cy = index/columns;
            tileset->SetScaleX(cxe);
            tileset->SetScaleY(cye);
            tileset->SetClip(GetTileWidth()*cx,GetTileHeight()*cy,GetTileHeight(),GetTileWidth());
            tileset->Render(x,y);
            tileset->SetScaleX(1);
            tileset->SetScaleY(1);
        }
    }
}

SDL_Texture *BlockTileSet::GetTexture(){
    return tileset->GetSDLTexture();

}


