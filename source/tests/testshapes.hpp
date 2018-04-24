#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/sprite.hpp"
#pragma once

class Test_Shapes: public State{
    public:
        Test_Shapes(){
            requestQuit = requestDelete = false;
            duration = 90.0f;
        };
        ~Test_Shapes(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Shapes");
            sp = Sprite("data/tiles.png");

        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){

            RenderHelp::DrawSquareColorA(Rect(32,32,64,64),255,255,100,255);
            RenderHelp::DrawSquareColorA(Rect(98,220,64,64),255,100,100,255,true);
            sp.SetClip(0,0,32,32);
            sp.Render(Point(32,32),0);
            sp.SetClip(64,64,32,32);
            sp.Render(Point(64,64),0);
            /*RenderHelp::DrawLineColor(96,96,128,128,255,0,255);
            RenderHelp::DrawLineColor(128,128,98,220,255,0,0,255,4);

            RenderHelp::DrawCircleColor(Point(98,220),32,100,100,255,100);*/
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Sprite sp;
        float duration;
};


