#pragma once
#include "../framework/vertex.hpp"
#include "shadermanager.hpp"
#include "sprite.hpp"


class PointSpriteTileset{
    public:
        PointSpriteTileset();
        void Render();
        void AddTile(Point pos, int id);
    private:
        VertexArrayObject m_vao;
        Shader m_psShader;
        Sprite m_textureAtlas;
        bool m_needBufferUpdate;
};
