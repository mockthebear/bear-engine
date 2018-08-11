#pragma once
#include "tilemap.hpp"
#include "../shadermanager.hpp"
#include "../targettexture.hpp"


class TargetTextureTileMap: public TileMap{
    public:


        TargetTextureTileMap():TileMap(){m_isValid = false; m_silence = false;};
        /**
            *Create the smart tileset and their textures. Create once, so you need all the settings right the way.
            @param tileSize The size of each tile. Example: (16,16)
            @param tilesetSize Tileset size, the amount of tiles in wich cordinate. Example: (50,90) So the map will have total 800 x 1440 pixels
            @param layers. In case your map has multiple layers. Default = 1
            @param maxTextureSize Max texture size. If your video card only support 120x120 textures, set it here  (or leave -1,-1 to automatic detection). Then the big texture will be split
            in many textures with the size of 120x120.
        */
        explicit TargetTextureTileMap(PointInt tileSize,PointInt3 tilesetSize,PointInt maxTextureSize=PointInt(-1,-1),bool silence = false);
        /**
            *This does delete all references. See the SmartTileset::operator=
        */
        ~TargetTextureTileMap(){};
        /**
            *Set an sprite to be the tileset image.
        */
        void SetTile(PointInt3 pos, Tile t);

        /**
            *Erase the current data in the position and replace the data with what you requested. Use as an single use.
            *If you need to call this multiple times, instead call SmartTileset::SetTile then SmartTileset::MakeMap once you finish

            @param layer the Layer
            @param x x position
            @param y y position
            @param tile tile

        */
        void SetTileDirect(PointInt3 pos, Tile t);
        /**
            Render an single layer.
            The render will start at the position 0,0 and only subtextures inside camera rect will be rendered.
            @param layer layer it
        */
        void RenderLayer(int layer,Point offset, Rect vision, bool showBoundary = false);
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

        void SetSilent(bool s){m_silence = s;};



        virtual void Render(Point offset, Rect vision=Rect(-1.0f));
        bool UpdateBuffers(){
            return MakeMap();
        };
        bool MakeMap();

        uint32_t CalculateSmallOffset(PointInt3 off);
    protected:
        TargetTexture& GetTextureFromPosition(int layer,int x,int y);
        PointInt m_framesOnMap;
        PointInt m_tilesetCompatSize;
        PointInt m_maxTextureSize;
        std::shared_ptr<TargetTexture> m_textureMap;
        std::shared_ptr<bool> m_needRemake;
        std::shared_ptr<bool> m_isUsed;
        bool m_isOnStream;
        bool m_isValid;
        bool m_silence;

        Sprite m_sp;
        PointInt m_sheetSizes;
};
