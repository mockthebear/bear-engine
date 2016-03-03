#ifndef TILEMAPH
#define TILEMAPH
#include "tileset.hpp"
class TileMap{
    public:
        TileMap (int mapWidth,int mapHeight,int layers,TileSet* tileSet=NULL);
        TileMap (char *str,TileSet* tileSet= NULL);
        void Load (char *file);
        void SetTileSet(TileSet* set);
        int& At (int x,int y,int z=0);
        void Render (int cameraX=0,int cameraY=0);
        void Render (int cx,int cy,int d);
        void RenderLayer (int layer=0,int cameraX=0,int cameraY=0);
        int GetWidth ();
        int GetHeight ();
        int GetDepth ();
        int getIndex(int x,int y,int z);
    private:
        std::vector<int> tileMatrix;
        TileSet* set;
        int mapWidth;
        int mapHeight;
        int mapDepth;
};

#endif
