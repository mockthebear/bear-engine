#pragma once

#include "libheader.hpp"
#include "../framework/geometry.hpp"
#include "../framework/chainptr.hpp"
#include "painters/painters.hpp"


class RenderTexture{
    public:
        RenderTexture():aliasing(TEXTURE_LINEAR),m_renderData(std::make_shared<RenderData>()),size(1.0f, 1.0f),TextureSize(1.0f, 1.0f){
             m_texture = Painter::UnloadedTexturePtr;
        };

        bool HasTexture();
        void SetTexture(TexturePtr t);
        RenderDataPtr GetRenderData();
        void SetRenderData(RenderDataPtr rd){m_renderData = rd;};
        void SetFlip(SDL_RendererFlip flipState);
        SDL_RendererFlip GetFlip();
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */
        void SetScaleX(float scale=1.0f);
        void SetScaleY(float scale=1.0f);
        /**
            *Change the sprite scale. Its an local scale, not shared.
            @param scale the original value is 1.
        */
        void SetScale(Point t_scale = Point(1.0f,1.0f));
        Point GetScale();
        /**
            *You can cut some color channels and reblend the sprite
            *This changes the texture, so its <b>SHARED</b>
            *When set something here, if you use the same sprite in another place, its good to restore
            @param Red [0-255] The default is 255 of all sprites;
            @param Blue [0-255] The default is 255 of all sprites;
            @param Green [0-255] The default is 255 of all sprites;
        */
        void ReBlend(uint8_t Red,uint8_t Blue,uint8_t Green);
        /**
            *Changed the sprite alpha
            *This changes the texture, so its <b>SHARED</b>
            *When set something here, if you use the same sprite in another place, its good to restore
            @param alpha [0-255] The default is 255 of all sprites;
        */
        void SetAlpha(uint8_t alpha);
        uint8_t GetAlpha();
        bool IsLoaded();
        /**
            *Get the texture. Note that texture can be an shared one
            @return The SDL_Texture pointer;
        */
        TexturePtr GetTexture();
        /**
            *Used to change the center of rotation
            @param p Sprite position (inside the image)
        */
        void SetCenter(Point p);
        Point GetCenter();
        Point GetSize(){ return size;};
        Point GetTextureSize(){ return TextureSize;};
        void RenderAll();
    protected:
        TextureLoadMethod aliasing;
        RenderDataPtr m_renderData;
        Point size;
        Point TextureSize;

        TexturePtr m_texture;
};
