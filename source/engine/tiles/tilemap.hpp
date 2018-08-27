#pragma once
#include "tileset.hpp"
#include <memory>
#include <iostream>


class Tile{
    public:
        Tile():id(0),flip(SDL_FLIP_NONE),rotate(0.0f){};
        Tile(uint32_t _id, uint8_t _rotate = 0,SDL_RendererFlip _flip = SDL_FLIP_NONE):id(_id),flip(_flip),rotate(_rotate){};
        Tile(uint32_t _id, uint8_t _rotate = 0,uint8_t _flip = 0):id(_id),flip(SDL_RendererFlip(_flip)),rotate(_rotate){};

        uint32_t id;
        SDL_RendererFlip flip;
        uint8_t rotate;
};

class TileMap: public TileSet{
    public:

        TileMap();
        /**
            X: width
            Y: height
            Z: layers
        */
        TileMap(PointInt tileSize,PointInt3 mapSize, Sprite set);


        virtual void Render(Point offset, Rect vision=Rect(-1.0f));
        virtual void SetTile(PointInt3 pos, Tile t);

        virtual uint32_t CalculateOffset(PointInt3 pos);

        void SetBlankTile(uint32_t id){
            m_blankTile = id;
        };

        PointInt3 GetMapSize(){
            return m_size;
        };


    protected:
        PointInt3 m_size;
        uint32_t m_blankTile;
        std::shared_ptr<Tile> m_map;
};
