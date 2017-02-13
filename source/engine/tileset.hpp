#ifndef TILESETH
#define TILESETH
#include "sprite.hpp"

#include <vector>


class TileSet{
    public:
        TileSet(){tileWidth=tileHeight=0;};
        virtual ~TileSet() {};
        virtual void Open (const char*s){};
        virtual void Render (int ind,int x,int y)=0;
        int GetTileWidth(){return tileWidth;};
        int GetTileHeight(){return tileHeight;};
        void SetTileHeight(int n){tileHeight=n;};
        void SetTileWidth(int n){tileWidth=n;};
    private:
        int tileWidth;
        int tileHeight;

};

class BlockTileSet: public TileSet{
    public:
        ~BlockTileSet(){};
        BlockTileSet(int tileWidth,int tileHeight,Sprite tset);
        void RawRender(int index,int x,int y);
        void Render(int index,int x,int y);
        void Render2(int index,int x,int y,float,float);
        SDL_Texture *GetTexture();
        Sprite tileset;
    private:

        int rows;
        int columns;

};

#endif
