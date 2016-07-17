#ifndef TILESETH
#define TILESETH
#include "sprite.hpp"

#include <vector>


class TileSet{
    public:
        ~TileSet(){tileWidth=tileHeight=0;};
        TileSet(){tileWidth=tileHeight=0;};
        virtual void Open (const char*s)=0;
        virtual void Render (int ind,int x,int y)=0;
        int GetTileWidth(){return tileWidth;};
        int GetTileHeight(){return tileHeight;};
        void SetTileHeight(int n){tileHeight=n;};
        void SetTileWidth(int n){tileWidth=n;};
    private:
        int tileWidth;
        int tileHeight;

};

class MultiTileSet: public TileSet{
    public:
        MultiTileSet(int tileWidth,int tileHeight);
        ~MultiTileSet();
        void Open (const char*);
        void Render (int index,int x,int y);
    private:
        std::vector<Sprite*> tileArray;
};

class BlockTileSet: public TileSet{
    public:
        BlockTileSet(int tileWidth,int tileHeight,char *file);
        ~BlockTileSet();
        void RawRender(int index,int x,int y);
        void Render(int index,int x,int y);
        void Render2(int index,int x,int y,float,float);
        SDL_Texture *GetTexture();
        void Open(const char *f);
    private:
        Sprite *tileset;
        int rows;
        int columns;

};

#endif
