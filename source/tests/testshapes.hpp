#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#pragma once

class Test_Shapes: public State{
    public:
        Test_Shapes(){
            requestQuit = requestDelete = false;
            duration = 30.0f;
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
            RenderHelp::DrawSquareColorA(Rect(32,32,64,64),255,255,100,255);
            RenderHelp::DrawLineColor(Point(96,96),Point(128,128),255,0,255);
            RenderHelp::DrawLineColor(Point(128,128),Point(98,220),255,0,0,255,4);
            RenderHelp::DrawSquareColorA(Rect(98,220,64,64),255,100,100,255,true);


            RenderHelp::DrawCircleColor(Point(98,220),32,100,100,255,100);
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
};


