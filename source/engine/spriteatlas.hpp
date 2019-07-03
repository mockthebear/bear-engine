#pragma once


#include "targettexture.hpp"


class SpriteAtlas{
    public:
        bool Generate(PointInt&& size);


        bool RenderSprite(PointInt&& position,uint32_t spriteId);
        bool RenderCall();

    private:
        bool BeginRenderCall();
        std::vector<Rect> m_sprites;
        TargetTexture m_atlas;
};
