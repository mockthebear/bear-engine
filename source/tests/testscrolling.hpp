#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/tiles/targettexturetilemap.hpp"
#pragma once

class Test_Scrolling: public State{
    public:
        Test_Scrolling(){
            requestQuit = requestDelete = false;
            bear::out << "Test target texture map scrolling\n";
            duration = 300.0f;
            Camera::Initiate();
        };
        ~Test_Scrolling(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test target texture map");
            follower = Rect(32,32,32,32);
            tset = TargetTextureTileMap(PointInt(32,32), PointInt3(160,120,2), PointInt(800,800), true);
            tset.SetSprite(Assets.make<Sprite>("data/tiles.png"));
            background = Assets.make<Sprite>("data/wall.jpg");
            for (int x=0;x<160;x++){
                for (int y=0;y<120;y++){
                    tset.SetTile(PointInt3(x, y, 0),Tile(rand()%170, rand()%4));
                    tset.SetTile(PointInt3(x, y, 1),Tile(rand()%170, rand()%4));
                }
            }
            tset.MakeMap();
            Camera::Follow(&follower,false);
            Camera::speed = 8.0f;
        };

        void Update(float dt){
            duration -= dt;
            tset.Update(dt);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }
            follower.x += 37.0 * dt;
            follower.y += 2.0 * dt;


            Camera::Update(dt);
        };
        void Render(){
            background.Render(0,0,0);

            tset.RenderLayer(0, Point(), Camera::pos);
            tset.RenderLayer(1, Point(), Camera::pos);

            RenderHelp::DrawSquareColor(Rect(follower.x - Camera::pos.x,follower.y - Camera::pos.y,follower.w,follower.h),0,0,0,255,true);
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Rect follower;
        Sprite background;
        TargetTextureTileMap tset;
        float duration;
};


