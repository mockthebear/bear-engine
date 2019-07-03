#pragma once
#include "painters/painters.hpp"


class TargetTexture : public BearTexture{
    public:
    TargetTexture():BearTexture(){
        std::cout << "11\n";
        m_renderData = AdvancedTransformations();
        std::cout << "22\n";
        m_vertexes = std::make_shared<VertexArrayObject>();
        std::cout << "33\n";
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
    bool IsValid(){
        return m_valid;
    }
    void Clear(BearColor c = BearColor(0.0f, 0.0f, 0.0f, 0.0f));

    AdvancedTransformations GetRenderData(){ return m_renderData;};

    private:
        unsigned int m_frameBuffer;
        unsigned int m_renderBuffer;
        VertexArrayObjectPtr m_vertexes;
        AdvancedTransformations m_renderData;
        bool m_valid;
};
