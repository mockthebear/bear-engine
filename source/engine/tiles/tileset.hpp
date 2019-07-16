#pragma once
#include "../sprite.hpp"
#include "../../framework/geometry.hpp"

#include <vector>

class TileSet{
    public:

        TileSet();

        TileSet(PointInt tileSize,Sprite set);

        virtual void SetSprite(Sprite sp);
        virtual Sprite &GetSprite(){return tileset;};

        virtual void RenderTile(int index,Point pos,uint8_t rotate=0,SDL_RendererFlip fliping = SDL_FLIP_NONE);

        Point GetTileSize();

    protected:
        Sprite tileset;
        Point m_tileSize;
        int m_rows;
        int m_columns;
        Point m_sheetSize;

};

