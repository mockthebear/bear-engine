#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#pragma once

class Test_Shapes: public State{
    public:
        Test_Shapes(){
            requestQuit = requestDelete = false;
            duration = 130.0f;
        };
        ~Test_Shapes(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Shapes");
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            RenderHelp::DrawLineColor(Point(32,32),g_input.GetMouse(),255,255,255,255,2);

            RenderHelp::DrawSquareColor(Rect(32,32,64,64),255,255,100,255 *(1.0f - (duration/130.0)),false, 360.f * (duration/130.0) );
            RenderHelp::DrawSquareColor(Rect(32,32,64,64),255,0,0,255,true);
            RenderHelp::DrawSquareColor(Rect(64,64,2,2),255,0,0,255);


            RenderHelp::DrawLineColor(Point(96,96),Point(128,128),255,0,255);
            RenderHelp::DrawLineColor(Point(128,128),Point(98,220),255,0,0,255,4);
            RenderHelp::DrawSquareColor(Rect(98,220,64,64),255,100,100,255,true);
            RenderHelp::DrawSquareColor(Rect(220,220,120,64),255,255,255,255,false);

            BearColor colors[4] = {
                BearColor(1.0f, 0.0f, 0.0f, 1.0f),
                BearColor(0.0f, 1.0f, 0.0f, 1.0f),
                BearColor(0.0f, 0.0f, 1.0f, 1.0f),
                BearColor(1.0f, 1.0f, 0.0f, 1.0f),
            };
            RenderHelp::DrawSquareColor(Rect(320,320,120,64),255,255,255,255,false, 0.0f, colors);



            std::vector<GameVertice> lines;
            std::vector<RectColor> squares;
            std::vector<RectColor> OutlineSquares;

            Point OldP(0, 0);
            BearColor OldC(1.0f, 1.0f, 1.0f, 1.0f);


            for (int i=0;i<20;i++){
                float radAngle = Geometry::toRad( (360.0f / 20.0f) * (float)(i+duration* 0.2f) * 3.0f  );

                float rColor = fabs(sin( radAngle ));
                float gColor = fabs(cos( radAngle ) + sin( radAngle ));
                float bColor = fabs(cos( radAngle ));
                float x = 64 + i * 32;
                float y = 128 + 32*sin( radAngle );
                if (i == 0){
                    OldP.x = x;
                    OldP.y = y;
                    OldC.r = rColor;
                    OldC.g = gColor;
                    OldC.b = bColor;
                }
                lines.emplace_back(GameVertice(OldP.x   ,   OldP.y  , OldC.r, OldC.b, OldC.g, 1.0f));
                lines.emplace_back(GameVertice(x        ,   y       , rColor, gColor, bColor, 1.0f));

                OutlineSquares.emplace_back(Rect(x-16,y-16,32,32));

                RectColor hecc( Rect(x-16,y-16,32,32), BearColor(0.15,0.15,0.6,0.5) );
                hecc.colors[0] = BearColor(0.5f, 1.0f, 1.0f, 1.0f);

                hecc.colors[2] = BearColor(0.0f, 0.5f, 0.0f, 1.0f);
                squares.emplace_back(hecc);

                OldP.x = x;
                OldP.y = y;
                OldC.r = rColor;
                OldC.g = gColor;
                OldC.b = bColor;
            }

            RenderHelp::DrawSquaresColor(squares);
            RenderHelp::DrawSquaresColor(OutlineSquares, 255, 255, 255, 150, true);
            RenderHelp::DrawLinesColor(lines,255,255,255,255,2);




            RenderHelp::DrawCircleColor(Point(98,220),32,100,100,255,100);
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
};

#endif // DISABLE_UNIT_TESTS
