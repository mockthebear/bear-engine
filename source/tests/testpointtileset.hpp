#pragma once
#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/pstileset.hpp"
#include "../engine/text.hpp"

class Test_PSTILESET: public State{
    public:
        Test_PSTILESET(){
            requestQuit = requestDelete = false;
            duration = 10.0f;
            m_tilesCount = 0.0f;
        };
        ~Test_PSTILESET(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test PSTILESET");
            lg = LineGraph(Point(500,200),200);
            tilesCount = Text("ui/UnB-Office_Regular.ttf",22,TEXT_SOLID,"a",{255,255,180});
            ps = new PointSpriteTileset();

        };

        void addTiles(){
            for (int i=0;i<300;i++){
                int x = rand()%30;
                int y = rand()%20;
                ps->AddTile(Point(x * 32,y * 32), rand()%150);
            }
            m_tilesCount += 300;
        }

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 ) {
                //requestDelete = true;
            }
            if (duration <= 0){
                lg.AddData(ScreenManager::GetInstance().GetFps());
                std::string txt = utils::format("FPS: %f  with %d tiles",ScreenManager::GetInstance().GetFps(), m_tilesCount );
                tilesCount.SetText(txt);
                addTiles();
                duration = 10.0f;
            }

        };
        void Render(){
            ps->Render();
            lg.Render(Point(32,32));
            tilesCount.Render(Point(82,12));
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        int m_tilesCount;
        float duration;
        PointSpriteTileset *ps;
        LineGraph lg;
        Text tilesCount;
};


