#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#pragma once

class Test_TargetTexture: public State{
    public:
        Test_TargetTexture(){
            requestQuit = requestDelete = false;

            duration = 100.0f;
            movement = 0;
        };
        ~Test_TargetTexture(){

        };
        void Begin(){
            //m_shader.Compile("engine/vertex.glvs","engine/lens.glfs");
            tileN = 174;
            tileAnim = 1.0f;
            ScreenManager::GetInstance().SetScreenName("Test target texture");
            bear::out << "Test target\n";
            background = Assets.make<Sprite>("data/wall.jpg");
            bear = Assets.make<Sprite>("data/bear.png");
            targ.Generate(400,400);

            example = Text("ui/UnB-Office_Regular.ttf",32,TEXT_SOLID,"Hello text",{33,33,120});
            Camera::Initiate();


        };

        void Update(float dt){

            movement += dt * 1.0f;
            duration -= dt;


            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {


                requestDelete = true;
            }


        };
        void Render(){
            background.SetRotation(0);
            background.Render(PointInt(0,0));


            targ.Bind();
                targ.Clear();
                background.SetRotation(45);
                background.Render(Point(0,0));
                RenderHelp::DrawCircleColor(Point(2,2),32,255,0,255,255);
                RenderHelp::DrawSquareColor(Rect(g_input.GetMouse().x, g_input.GetMouse().y, 32,32),255,0,0,255,true);
                RenderHelp::DrawCircleColor(Point(400-32,400-32),32,255,255,255,255);
                bear.Render(Point(150,150));
                example.Render(Point(16,16));
            targ.UnBind();

            targ.Render(Point(32 + movement,32));
            RenderHelp::DrawSquareColor(Rect(32 + movement,32,400,400),0,0,0,255,true);
            RenderHelp::DrawSquareColor(Rect(g_input.GetMouse().x, g_input.GetMouse().y, 32,32),255,0,0,255,true);


        };
        void Input();
        void Resume(){};
        void End(){
            m_shader.Close();
        };
    private:
        float movement;
        Shader m_shader;
        Sprite background;
        Sprite bear;
        float duration;
        float tileAnim;
        int tileN;
        Text example;
        TargetTexture targ;
};
#endif // DISABLE_UNIT_TESTS
