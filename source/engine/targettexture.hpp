#pragma once
#include "painters/painters.hpp"

class SpriteAtlas;

class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        m_renderData = AdvancedTransformations();
        m_vertexes = std::make_shared<VertexArrayObject>();
        m_valid = false;
    };

    bool Generate(int w,int h);
    bool Bind();
    bool BindTexture(bool bind=true);
    bool BindRenderBuffer(bool bind=true);
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){m_renderData.SetScale(p);};
    void SetAlpha(uint8_t alpha){m_renderData.defaultColor.a = alpha/255.0f;};
    void SetAlpha(float alpha){m_renderData.defaultColor.a = alpha;};

    void ReBlend(uint8_t Red,uint8_t Blue,uint8_t Green){
        m_renderData.defaultColor[0] = Red/255.0f;
        m_renderData.defaultColor[1] = Blue/255.0f;
        m_renderData.defaultColor[2] = Green/255.0f;

    };

    bool IsValid(){
        return m_valid;
    }
    void Clear(BearColor c = BearColor(0.0f, 0.0f, 0.0f, 0.0f));

    AdvancedTransformations GetRenderData(){ return m_renderData;};

    private:
        friend class SpriteAtlas;
        unsigned int m_frameBuffer;
        unsigned int m_renderBuffer;
        VertexArrayObjectPtr m_vertexes;
        AdvancedTransformations m_renderData;
        bool m_valid;
};
