#pragma once


#include "targettexture.hpp"
#include "sprite.hpp"


class SpriteAtlas{
    public:

        bool Generate(PointInt&& size);

        bool RenderWholeAtlas(PointInt pos);
        bool RenderAll();

        int StichSprite(RenderTexture &rt);
        int StichSprite(std::string name);
        int StichSprite(const char *name);

        void OpenStich();
        void CloseStich();

        Sprite SummonSprite(uint32_t spriteId);


    private:
        bool checkValidSpot(RectInt &guessPosition,PointInt &maxLocations);
        std::vector<RectInt> m_sprites;
        TargetTexture m_atlas;
        VertexArrayObjectPtr m_vertexes;
        PointInt m_size;
        uint32_t m_LowestY,m_GridSize;

};
