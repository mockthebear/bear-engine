#include "tilemap.hpp"
#include <stdio.h>

TileMap::TileMap(int mw,int mh,int ly,TileSet *s){
    tileMatrix.resize(mw*mh*ly);
    for(int i =tileMatrix.size() - 1; i >= 0; --i){
        tileMatrix[i] = -1;
    }
    mapWidth = mw;
    mapHeight = mh;
    mapDepth = ly;
    set = s;
}

TileMap::TileMap (char *str,TileSet* s){
    set = s;
    Load(str);
}

void TileMap::Load(char *str){
	FILE *f = fopen(str,"r");
    if (f){
		fscanf(f,"%d,%d,%d,",&mapWidth,&mapHeight,&mapDepth);
        printf("%d,%d,%d,\n",mapWidth,mapHeight,mapDepth);
        tileMatrix.resize(mapWidth*mapHeight*mapDepth);
        for(int i =tileMatrix.size() - 1; i >= 0; --i){
            tileMatrix[i] = -1;
        }

        for (int d=0;d<mapDepth;d++){
            for (int i=0;i<mapWidth;i++){
                for (int j=0;j<mapHeight;j++){
                   fscanf(f,"%d,",&tileMatrix[getIndex(j,i,d)]);
                   tileMatrix[getIndex(j,i,d)]--;

                }
            }
        }
        fclose(f);

    }else{
        printf("Cannot load %s\n",str);
    }
}

void TileMap::SetTileSet(TileSet* s){
    set = s;
}
int& TileMap::At (int x,int y,int z){
    return (tileMatrix[getIndex(x,y,z)]);
}
int TileMap::getIndex(int x,int y,int z){
    return (z*mapWidth*mapHeight)+(y*mapWidth)+x;
}
void TileMap::Render (int cx,int cy){

        int ww = set->GetTileWidth();
        int hh = set->GetTileHeight();

        for (int d = 0;d<mapDepth;d++){
            for (int w=0;w<mapWidth;w++){
                for (int h=0;h<mapHeight;h++){
                    set->Render(tileMatrix[getIndex(w,h,d)],w*ww-cx,h*hh-cy);
                }
            }
        }
}

void TileMap::Render (int cx,int cy,int d){

        int ww = set->GetTileWidth();
        int hh = set->GetTileHeight();


            for (int w=0;w<mapWidth;w++){
                for (int h=0;h<mapHeight;h++){

                    set->Render(tileMatrix[getIndex(w,h,d)],w*ww-cx,h*hh-cy);
                }
            }

}
