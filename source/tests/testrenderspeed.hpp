#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



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
            duration = 20.0f;
            state = 0;
            lg = LineGraph(Point(500,200),200);
            m_renderTimer = LineGraph(Point(500,200),200);
            tilesCount = Text("ui/UnB-Office_Regular.ttf",22,TEXT_SOLID,"a",{255,33,180});

        };
        ~Test_RenderSpeed(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Sprite Render Speed");
            bear::out << "Testing Load sprite from an direct file:\n";



            tiles = Assets.make<Sprite>("data/tiles.png");
            tiles.SetGrid(16,16);


        };



        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 ) {
                requestDelete = true;
            }

            if (state >= 2000){
                return;
            }
            static int owo = 0;
            if (duration <= 0){
                duration = 0.1f;
                state++;


                if (state % 10 == 0 && state > 0){
                     owo++;

                    lg.AddData(ScreenManager::GetInstance().GetFps());
                    m_renderTimer.AddData(renderDuration / (double)state);
                    std::string txt = utils::format("FPS: %f  : Time to render one call: %f",ScreenManager::GetInstance().GetFps(), renderDuration / (double)state );
                    tilesCount.SetText(txt);
                     if (owo >= 4){
                        bear::out << txt << "\n";
                     }

                }

            }



        };
        void Render(){
            sw.Reset();
            RenderHelp::DrawSquareColor(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),100,100,100,255);
            for (int i=0;i<state;i++){
                tiles.SetClip((rand()%10) * 16,(rand()%10) * 16,16,16);
                for (int a=1;a<10;a++){
                    tiles.Render(Point( (i % 60) * 16, (i/60)*16 )) ;
                }
            }
            //Painter::DrawSprites(tiles.GetTexture().get()->id);
            renderDuration = (double)sw.Get();

            lg.Render(Point(32,32));
            m_renderTimer.Render(Point(32,264));

            tilesCount.Render(Point(64,64));

        };
        void Input();
        void Resume(){};
        void End(){
            Assets.erase();
            ResourceManager::GetInstance().Erase("test");
        };
    private:
        double renderDuration;
        Stopwatch sw;
        LineGraph lg;
        LineGraph m_renderTimer;
        Sprite tiles;
        Text tilesCount;
        int state;
        float duration;
};


#endif // RUN_TESTS


