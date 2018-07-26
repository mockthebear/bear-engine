#pragma once
#include "../framework/vertex.hpp"
#include "shadermanager.hpp"
#include "sprite.hpp"


class PointSpriteTileset{
    public:
        PointSpriteTileset();
        void Render();
    private:
        VertexArrayObject m_vao;
        Shader m_psShader;
        Sprite m_textureAtlas;
        unsigned int VBO, VAO;
};
