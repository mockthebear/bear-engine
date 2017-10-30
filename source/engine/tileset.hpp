#ifndef TILESETH
#define TILESETH
#include "sprite.hpp"

#include <vector>

/**
    @brief Basic tileset class for small tilesets
    * If you are using big tilesets, its better use SmartTileset
    *
    * <B>This class won't store any tile data!</B>
    *
    * In bear engine all tileset works in this way:
    *  - An sprite has like 100x100 pixels.
    *  - Its set each tile have 10x10 pixels
    * Now its sent to TileSet::Render with parameters 14 and some x,y cordinates.
    * Its given, each tile have 10x10, the selected tile will be in the position 10 x 40 in the image
    * Basically: X = Index / columns; Y = Index % columns
*/

class TileSet{
    public:
        /** Constructor
         * Empty constructor. Initialize everything with preset 16x16 size
         */
        TileSet();
        /** Constructor
         *
         * @param tileWidth The tile width in pixels
         * @param tileHeight The tile height in pixels
         * @param tset The tileset image. Will be used to define the amount of rows and columns
         */
        TileSet(int tileWidth,int tileHeight,Sprite tset);
        /** Query an sprite
         * @param sp The tileset image. Will be used to define the amount of rows and columns
         */
        void SetSprite(Sprite &sp);
        /** Render an tile
         * @param index Tile index
         * @param pos an Point vector containing x,y position
         */
        void Render(int index,Point pos);
        /** Render an tile scaling it
         * @param index Tile index
         * @param pos an Point vector containing x,y position
         * @param scaleX X scale factor
         * @param scaleY Y scale factor
         */
        void RenderScaled(int index,Point pos,float scaleX,float scaleY);
        /** Return the tile sile
         * @return Point the sizes
         */
        Point GetTileSize();
    private:
        Sprite tileset;
        int m_tileWidth;
        int m_tileHeight;
        int m_rows;
        int m_columns;

};

#endif
