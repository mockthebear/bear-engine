#pragma once
#include "../framework/geometry.hpp"
#include "gamebase.hpp"
#include "sprite.hpp"

class SmartTileset{
    public:
        SmartTileset(){tileSize= PointInt(8,8);};
        SmartTileset(PointInt tileSize,PointInt tilesetSize,int layers=1,PointInt maxTextureSize=PointInt(-1,-1));
        ~SmartTileset();
        void SetSprite(Sprite spr);
        bool MakeMap();
        void SetTile(int layer,int x,int y,int tile);
        void SetTileDirect(int layer,int x,int y,int tile);
        void RenderLayer(int layer);
        void Update(float dt);
        void ResetFocus();
        SmartTileset& operator=(SmartTileset T){
            /*
            shallow copy
            */
            void *data = (void*)&T;
            memcpy(this,data,sizeof(SmartTileset));
            T.isValid = false;
            return *this;
        }
    private:

        void RenderTile(int x,int y,int tile);
        SDL_Texture *GetTextureFromPosition(int layer,int x,int y);
        PointInt framesOnMap;
        PointInt tileSize;
        PointInt tilesetSize;
        PointInt tilesetCompatSize;
        PointInt maxTextureSize;
        int Layers;
        int    ***tileMap;
        SDL_Texture ****textureMap;
        SDL_Texture *lastTarget;
        bool ***needRemake;
        bool isOnStream;
        bool isValid;


        //Sprite data:

        Sprite sp;
        PointInt sheetSizes;
};
