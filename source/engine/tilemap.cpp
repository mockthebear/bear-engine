#include "tilemap.hpp"
#include <stdio.h>

TileMap::TileMap(){
    mapWidth = mapHeight = mapDepth = 0;
}

TileMap::TileMap(int _mapWidth,int _mapHeight,int layers,TileSet tileSet):TileMap(){

    mapWidth = _mapWidth;
    mapHeight = _mapHeight;

    set = tileSet;
    tileMatrix = utils::Mat3<int>(mapWidth,mapHeight,mapDepth);

}

void TileMap::SetTileSet(TileSet s){

    set = s;

}

int& TileMap::At (int x,int y,int z){

    return tileMatrix.at(x,y,z);
}

void TileMap::Render(Point pos){

    Point tSize = set.GetTileSize();

    for (int d = 0;d<mapDepth;d++){
        for (int w=0;w<mapWidth;w++){
            for (int h=0;h<mapHeight;h++){

                Point auxPos(w,h);
                auxPos = auxPos * tSize;
                auxPos = auxPos + pos;
                set.Render(tileMatrix.at(w,h,d),auxPos);

            }
        }
    }
}

void TileMap::RenderLayer(Point pos,int layer){

    Point tSize = set.GetTileSize();

    for (int w=0;w<mapWidth;w++){
        for (int h=0;h<mapHeight;h++){

            Point auxPos(w,h);
            auxPos = auxPos * tSize;
            auxPos = auxPos + pos;
            set.Render(tileMatrix.at(w,h,layer),auxPos);

        }
    }

}
