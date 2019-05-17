#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS


#include "../engine/shadermanager.hpp"
#include "../engine/shadermanager.hpp"
#include "../framework/light2d.hpp"

#pragma once





class Test_NewLight: public State{
    public:
        Test_NewLight(){
            requestQuit = requestDelete = false;
            bear::out << "Test new light\n";
            duration = 130.0f;


            opaqueObjs.emplace_back(Rect(200, 200,  50, 50));
            opaqueObjs.emplace_back(Rect(100, 100,  50, 50));
            opaqueObjs.emplace_back(Rect(300, 400,  80, 50));
            opaqueObjs.emplace_back(Rect(100, 200,  80, 160));
            opaqueObjs.emplace_back(Rect(32, 32, 32, 32));
            opaqueObjs.emplace_back(Rect(96, 32, 32, 32));
            opaqueObjs.emplace_back(Rect(180, 32, 32, 32));

        };
        ~Test_NewLight(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test new light");
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }


            bg = Sprite("test:wall.jpg");
           led = Sprite("data/led.png");



            m_light.Start(ScreenManager::GetInstance().GetGameSize(), 1.0f);

            for (auto &it : opaqueObjs){
                m_light.StoreOpaque(it);
            }
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            m_light.ClearCanvas();
            Point mpos = g_input.GetMouse();
            bg.Render(PointInt(0,0));

            m_light.SetLight(mpos, 300,  BearColor(0.0f, 0.0f, 0.0f, 1.0f));

            led.ReBlend(255, 0, 0);
            led.Render(Point(400,400) - Point(8,8));
            m_light.SetLight(Point(400,400), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend(0, 0, 255);
            led.Render(Point(400,200) - Point(8,8));
            m_light.SetLight(Point(400,200), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend(0, 255, 0);
            led.Render(Point(80,240) - Point(8,8));
            m_light.SetLight(Point(80,240), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend(255, 255, 255);
            led.Render(Point(330,480) - Point(8,8));
            m_light.SetLight(Point(330,480), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend(0.4f * 255, 0.4f * 255, 255);
            led.Render(Point(480,480) - Point(8,8));
            m_light.SetLight(Point(480,480), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend( 255,  255, 255);
            led.Render(Point(580,480) - Point(8,8));
            m_light.SetLight(Point(580,480), 100, BearColor(0.0f, 0.0f, 0.0f, 1.0f)) ;

            led.ReBlend(255, 255, 255);
            led.Render(mpos - Point(8,8));



            for (auto &it : opaqueObjs){
                RenderHelp::DrawSquareColor(it, 255, 200,100, 255);
            }


            m_light.Render();

        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Light2D m_light;
        float duration;
        Sprite bg;
        Sprite led;

        std::vector<Rect> opaqueObjs;
};

#endif // DISABLE_UNIT_TESTS

