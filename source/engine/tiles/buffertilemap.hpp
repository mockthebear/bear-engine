#pragma once
#include "tilemap.hpp"
#include "../shadermanager.hpp"
#include "../../glm/glm.hpp"


class BufferTileVAO: public VertexArrayObject{
    public:
        BufferTileVAO():VertexArrayObject(){}

        virtual void SetAttributes();
};


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
        static Shader m_shader;
    protected:
        void BindBuffer();
        void UpdateModel();
        void SetupAttributes();
        void InternalAddTile(uint32_t id, uint8_t rotate, SDL_RendererFlip flip, Point pos);
        uint32_t elemCount;
        BufferTileVAO m_vao;

        AdvancedTransformations m_renderData;
        BearColor m_color;
        Point m_scale;
        glm::mat4 m_tileModel;

        static bool m_madeShaders;
        static float forwardSquares[4][8];
};
