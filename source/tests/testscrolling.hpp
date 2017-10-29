#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/smarttileset.hpp"
#pragma once

class Test_Scrolling: public State{
    public:
        Test_Scrolling(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 200.0f;
            Camera::Initiate();
        };
        ~Test_Scrolling(){

        };
        void Begin(){
            follower = Rect(32,32,32,32);
            tset = new SmartTileset(Point(32,32),Point(160,120),2,Point(800,800));
            tset->SetSprite(Assets.make<Sprite>("data/tiles.png"));
            background = Assets.make<Sprite>("data/wall.jpg");
            for (int i=0;i<160;i++){
                tset->SetTile(0,i,0,110);
                tset->SetTile(0,i,i,110);
            }
            //tset->MakeMap();
            Camera::Follow(&follower,false);
            Camera::speed = 8.0f;
        };

        void Update(float dt){
            duration -= dt;
            tset->Update(dt);
            if(  duration <= 0 ) {
                requestDelete = true;
            }
            follower.x += 37.0 * dt;


            Camera::Update(dt);
        };
        void Render(){
            background.Render(0,0,0);

            tset->RenderLayer(0);
            tset->RenderLayer(1);

            RenderHelp::DrawSquareColor(Rect(follower.x - Camera::pos.x,follower.y - Camera::pos.y,follower.w,follower.h),0,0,0,255,true);
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Rect follower;
        Sprite background;
        SmartTileset *tset;
        float duration;
};


