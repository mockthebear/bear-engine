#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        m_renderData = std::make_shared<RenderData>();
        m_valid = false;
    };

    bool Generate(int w,int h);
    bool Bind();
    bool BindTexture(bool bind=true);
    bool BindRenderBuffer(bool bind=true);
    static bool UnBind();
    void Render(Point pos);
    bool FreeTexture();
    void SetScale(Point p){m_renderData->SetScale(p);};
    bool IsValid(){
        return m_valid;
    }
    void Clear(BearColor c = BearColor(0.0f, 0.0f, 0.0f, 0.0f));

    RenderDataPtr GetRenderData(){ return m_renderData;};

    private:
        unsigned int m_frameBuffer;
        unsigned int m_renderBuffer;
        RenderDataPtr m_renderData;
        bool m_valid;
};
