#pragma once

#include "../engine/shadermanager.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/targettexture.hpp"

class LineData{
            public:
                LineData(){}
                LineData(Line l, float r){
                    rot = r;
                    line = l;
                }
                Line line;
                float rot;
        };

class Light2D{
    public:
        Light2D(){
        };

        ~Light2D();


        void Start(PointInt size, float maxDark = 0.07f);

        void StoreOpaque(Rect objBox);
        void ClearOpaque();

        void ClearCanvas();
        void SetLight(Point pos, float radius, BearColor color = BearColor(1.0f, 1.0f, 1.0f, 1.0f), float minRays = 8.0f);
        void Render(Point pos = Point(0.0f, 0.0f));

    private:

        void RenderLineData(std::vector<LineData> &ld, Point pos, float radius, BearColor& color);
        float m_sizeMultiplier;
        float m_maxDark;
        PointInt m_size;
        Shader m_haloShader;
        Shader m_invertShader;
        TargetTexture m_targetTexture;
        std::vector<Rect> m_opaqueObjs;


};
