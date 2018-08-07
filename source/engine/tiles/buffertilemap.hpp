#pragma once
#include "tilemap.hpp"
#include "../shadermanager.hpp"
#include "../../glm/glm.hpp"




class BufferTileMap: public TileMap{
    public:

        BufferTileMap(){
            elemCount = 0;
        };
        /**
            X: width
            Y: height
            Z: layers
        */
        BufferTileMap(PointInt tileSize,PointInt3 mapSize, Sprite set);

        Point GetTileSize();


        virtual void Render(Point offset, Rect vision=Rect(-1.0f));

        static void checkShaders();

        void UpdateBuffers();

    protected:
        void BindBuffer();
        void UpdateModel();

        void InternalAddTile(uint32_t id, uint8_t rotate, SDL_RendererFlip flip, Point pos);


        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
        uint32_t elemCount;

        uint32_t VertexBuffer;
        uint32_t ElementBuffer;

        BasicRenderData m_renderData;
        BearColor m_color;
        Point m_scale;
        glm::mat4 m_tileModel;

        static Shader m_shader;
        static bool m_madeShaders;
};
