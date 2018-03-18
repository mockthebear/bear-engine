#include "../settings/definitions.hpp"
#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../engine/sprite.hpp"
#include "../engine/text.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/shadermanager.hpp"

#include "../framework/resourcemanager.hpp"


class Test_RenderSpeed: public State{
    public:
        Test_RenderSpeed():State(){

            requestQuit = requestDelete = false;
            duration = 10.0f;
            state = 0;
            lg = LineGraph(Point(500,200),200);
            tilesCount = Text("ui/UnB-Office_Regular.ttf",22,TEXT_SOLID,"a",{255,33,180});
            tilesCount.RemakeTexture();

        };
        ~Test_RenderSpeed(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Render Speed");
            bear::out << "Testing Load sprite from an direct file:\n";



            tiles = Assets.make<Sprite>("data/tiles.png");
            tiles.SetGrid(16,16);


        };



        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed()  ) {
                requestDelete = true;
            }
            if (duration <= 0){
                duration = 0.1f;
                state++;
                if (state % 10 == 0 && state > 0){
                    lg.AddData(ScreenManager::GetInstance().GetFps());
                    std::string txt = utils::format("%d  : %f",state, renderDuration / (float)state );
                    tilesCount.SetText(txt);

                }
                if (state >= 800){
                    requestDelete = true;
                }
            }

        };
        void Render(){
            sw.Reset();
            RenderHelp::DrawSquareColor(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),100,100,100,255);
            for (int i=0;i<state;i++){
                for (int a=1;a<10;a++){
                    tiles.SetClip((rand()%10) * 16,(rand()%10) * 16,16,16);
                    tiles.Render(Point( (i % 60) * 16, (i/60)*16 )) ;
                }
            }
            renderDuration = sw.Get()/1000.0f;
            lg.Render(Point(32,32));

            tilesCount.Render(Point(64,64));

        };
        void Input();
        void Resume(){};
        void End(){
            Assets.erase();
            ResourceManager::GetInstance().Erase("test");
        };
    private:
        float renderDuration;
        Stopwatch sw;
        LineGraph lg;
        Sprite tiles;
        Text tilesCount;
        int state;
        float duration;
};




