#pragma once
#include "../framework/geometry.hpp"
#include "gamebase.hpp"
#include "sprite.hpp"
#include "renderhelp.hpp"

/**
    @brief Fast rendering optmized tileset.

    * This tileset generate an series of textures, create the tileset on there once and update only section IF
    * there is any kind of animation.
    *
    * Tileset done on the old way with an matrix and every frame rendering all tiles are slow.
    * this class is optmized to do it in a fast way and rendering only wich is necessary when is necessary.
    *
    * The tileset is organized in the following way
    * Given the tiles are 16x16, if you set the tile 4, it means in tileset textur, the tile 4, will be at position X = 16 * 4, Y = 0.
    * But if lets say, the texture are on the size 160 x 1600, it will means the tile 10, will actually be at position X = 0 and Y = 1.
    * This will be automatic.
*/
class TileInfo{
    public:
        TileInfo():id(0),rotation(0),flip(SDL_FLIP_NONE){};
        int id;
        int rotation;
        SDL_RendererFlip flip;
};

class SmartTileset{
    public:
        /**
            Empty constructor.
        */
        SmartTileset(){tileSize= PointInt(8,8);};
        /**
            *Create the smart tileset and their textures. Create once, so you need all the settings right the way.
            @param tileSize The size of each tile. Example: (16,16)
            @param tilesetSize Tileset size, the amount of tiles in wich cordinate. Example: (50,90) So the map will have total 800 x 1440 pixels
            @param layers. In case your map has multiple layers. Default = 1
            @param maxTextureSize Max texture size. If your video card only support 120x120 textures, set it here  (or leave -1,-1 to automatic detection). Then the big texture will be split
            in many textures with the size of 120x120.
        */
        explicit SmartTileset(PointInt tileSize,PointInt tilesetSize,int layers=1,PointInt maxTextureSize=PointInt(-1,-1));
        /**
            *This does delete all references. See the SmartTileset::operator=
        */
        ~SmartTileset();
        /**
            *Set an sprite to be the tileset image.
        */
        void SetSprite(Sprite spr);
        /**
            *Force the object to remake all textures by the given sprite.
        */
        bool MakeMap();
        /**
            *Set an given tile on a given layer on a given positio to be what you set. This wont remake any textures.
            @param layer the Layer
            @param x x position
            @param y y position
            @param tile tile
        */
        void SetTile(int layer,int x,int y,int tile,double angle=0,SDL_RendererFlip fliping = SDL_FLIP_NONE);
        /**
            *Erase the current data in the position and replace the data with what you requested. Use as an single use.
            *If you need to call this multiple times, instead call SmartTileset::SetTile then SmartTileset::MakeMap once you finish

            @param layer the Layer
            @param x x position
            @param y y position
            @param tile tile

        */
        void SetTileDirect(int layer,int x,int y,int tile, double angle=0,SDL_RendererFlip fliping = SDL_FLIP_NONE);
        /**
            Render an single layer.
            The render will start at the position 0,0 and only subtextures inside camera rect will be rendered.
            @param layer layer it
        */
        void RenderLayer(int layer,bool showBoundary = false);
        /**
            *In some sdl versions, whenever you resize the sceen, textures with SDL_TEXTUREACCESS_TARGET get reseted in some drivers
            *So this will check for you if the screen has been resized or any need to remake the textures.
            *Better call always
        */
        void Update(float dt);
        /**
            Basically calls
            @code
            ScreenManager::GetInstance().SetRenderTarget(nullptr);
            @endcode
        */
        void ResetFocus();
        /**
            Its an shallow coppy with an differential, the old one got the flag isValid set to false.
        */
        SmartTileset& operator=(SmartTileset T){
            void *data = (void*)&T;
            memcpy(this,data,sizeof(SmartTileset));
            T.isValid = false;
            return *this;
        }
    private:

        void RenderTile(int x, int y, TileInfo &t);
        TargetTexture*& GetTextureFromPosition(int layer,int x,int y);
        PointInt framesOnMap;
        PointInt tileSize;
        PointInt tilesetSize;
        PointInt tilesetCompatSize;
        PointInt maxTextureSize;
        int Layers;
        TileInfo    ***tileMap;
        TargetTexture ****textureMap;
        TargetTexture *lastTarget;
        bool ***needRemake;
        bool ***m_isUsed;
        bool isOnStream;
        bool isValid;


        //Sprite data:

        Sprite sp;
        PointInt sheetSizes;
};
