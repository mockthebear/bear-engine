#ifndef TILEMAPH
#define TILEMAPH
#include "tileset.hpp"
#include "../framework/utils.hpp"
/**
    @brief Tile map class
    *
    * This class does store tile information. Only its ids
    *
    *
    *
*/
class TileMap{
    public:
        /**
        * Empty constructor
        */
        TileMap();
        /**
        * Create an tilemap matrix with given sizes.
        * @param mapWidth its width
        * @param mapHeight its width
        * @param layers amount of layers. Minimum is 1
        * @param tileSet An tileset object.
        */
        TileMap(int mapWidth,int mapHeight,int layers,TileSet tileSet);
        /**
        * Set the given tileset. Can be switched at any time
        */
        void SetTileSet(TileSet tset);
        /**
        * @param x The x position in the tile matrix
        * @param y The y position in the tile matrix
        * @param z The Layer
        * @return The reference of the current tile. Can be modified
        */
        int& At (int x,int y,int z=0);
        /** Render all layers
        * @param pos The position of the start (top left) position of the whole tileset
        */
        void Render(Point pos);
        /** Render a single layer
        * @param pos The position of the start (top left) position of the whole tileset
        */
        void RenderLayer(Point pos,int layer);
        /** Get the size
        * @return The current width
        */
        int GetWidth();
        /** Get the size
        * @return The current  height
        */
        int GetHeight();
        /** Get the size
        * @return The current layer count
        */
        int GetDepth();
    private:
        utils::Mat3<int> tileMatrix;
        TileSet set;
        int mapWidth;
        int mapHeight;
        int mapDepth;
};

#endif
