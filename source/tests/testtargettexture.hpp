
#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/smarttileset.hpp"
#pragma once

class Test_TargetTexture: public State{
    public:
        Test_TargetTexture(){
            requestQuit = requestDelete = false;
            tset = nullptr;
            duration = 100.0f;
            movement = 0;
        };
        ~Test_TargetTexture(){
            if (tset){
                delete tset;
            }
        };
        void Begin(){
            //m_shader.Compile("engine/vertex.glvs","engine/lens.glfs");
            tileN = 174;
            tileAnim = 1.0f;
            ScreenManager::GetInstance().SetScreenName("Test target texture");
            bear::out << "Test target\n";
            background = Assets.make<Sprite>("data/wall.jpg");
            targ.Generate(400,400);
            tset = new SmartTileset(Point(32,32),Point(15,15),2,Point(320,320));
            tset->SetSprite(Assets.make<Sprite>("data/tiles.png"));
            Camera::Initiate();
            tset->SetTile(0,0,0,110);
            tset->SetTile(0,2,0,111);
            tset->SetTile(0,0,3,112);
            tset->SetTile(0,0,4,113);
            tset->SetTile(0,0,5,110);
            tset->SetTile(0,0,6,110);
            tset->SetTile(0,0,7,110);
            tset->SetTile(0,1,7,120);
            tset->SetTile(0,2,7,174);
            tset->SetTile(0,3,7,120);
            tset->SetTile(1,0,8,110);
            tset->SetTile(0,18,8,110);
            tset->SetTile(1,0,7,117);

            tset->MakeMap();

        };

        void Update(float dt){
            tset->Update(dt);
            movement += dt * 1.0f;
            duration -= dt;
            tileAnim -= dt;
            if (tileAnim <= 0){
                tileAnim = 1.0f;
                tileN++;
                tset->SetTileDirect(0,2,7,tileN);
                if (tileN >= 174+5){
                    tileN = 174;
                }
            }

            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {


                requestDelete = true;
            }


        };
        void Render(){
            background.Render(0,0,0);


            targ.Bind();
            background.Render(Point(0,0),45);

            RenderHelp::DrawCircleColor(Point(400-64,400 - 64),32,255,0,255,255);



            RenderHelp::DrawCircleColor(Point(32,32),32,255,255,255,255);
            RenderHelp::DrawCircleColor(Point(400-32,400-32),32,255,255,255,255);



            targ.UnBind();

            targ.Render(Point(32 + movement,32));

            tset->RenderLayer(0);
            tset->RenderLayer(1);



        };
        void Input();
        void Resume(){};
        void End(){
            m_shader.Close();
        };
    private:
        float movement;
        Shader m_shader;
        SmartTileset *tset;
        Sprite background;
        float duration;
        float tileAnim;
        int tileN;
        TargetTexture targ;
};

