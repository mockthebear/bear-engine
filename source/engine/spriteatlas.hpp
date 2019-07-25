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
        Sprite SummonSprite(std::string name);

        int GetId(std::string name);


    private:
        bool checkValidSpot(RectInt &guessPosition,PointInt &maxLocations);
        std::vector<RectInt> m_sprites;
        std::map<std::string, int> m_spritIdsName;
        TargetTexture m_atlas;
        VertexArrayObjectPtr m_vertexes;
        PointInt m_size;
        uint32_t m_LowestY,m_GridSize;

};
