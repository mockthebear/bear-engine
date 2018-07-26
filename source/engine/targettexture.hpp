#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        m_renderData = std::make_shared<RenderData>();
    };
    bool Generate(int w,int h);
    bool Bind();
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){m_renderData->SetScale(p);};

    RenderDataPtr GetRenderData(){ return m_renderData;};

    private:
        unsigned int m_frameBuffer;
        unsigned int m_renderBuffer;
        RenderDataPtr m_renderData;
};
