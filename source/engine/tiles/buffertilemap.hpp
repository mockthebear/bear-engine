#pragma once
#include "tilemap.hpp"
#include "../shadermanager.hpp"
#include "../../glm/glm.hpp"

class BufferTileMap: public TileMap{
    public:

        BufferTileMap():TileMap(),m_vertexes(std::make_shared<VertexArrayObject>()){
        };
        /**
            X: width
            Y: height
            Z: layers
        */
        BufferTileMap(PointInt tileSize,PointInt3 mapSize, Sprite set);
        Point GetTileSize();

        virtual void Render(Point offset, Rect vision=Rect(-1.0f));
        void UpdateBuffers();
    protected:

        void InternalAddTile(uint32_t id, uint8_t rotate, SDL_RendererFlip flip, Point pos);

        VertexArrayObjectPtr m_vertexes;
        AdvancedTransformations m_renderData;

};
