#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../sound/sound.hpp"
#pragma once

class Test_Sound: public State{
    public:
        Test_Sound(){
            requestQuit = requestDelete = false;
            bear::out << "Test sound\n";
            duration = 100.0f;
        };
        ~Test_Sound(){

        };
        void Begin(){
            bgm = Sound("data/yay3.wav");
            ScreenManager::GetInstance().SetScreenName("Test sound");
            soundInfo = Text("Sound?",22,{255,255,180});
            bgm.Play(-1);
            //getchar();
            //
            Assets.erase();

        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            soundInfo.SetText( utils::format("Duration: %f / %f",bgm.GetPosition(),bgm.GetDuration()) );

        };
        void Render(){
            soundInfo.Render(Point(SCREEN_SIZE_W/2,SCREEN_SIZE_H/2),TEXT_RENDER_CENTER);
        };
        void Input();
        void Resume(){};
        void End(){
            bear::out << "Rip sound\n";
            bgm.Kill();

        };
    private:
        Sound bgm;
        Text soundInfo;
        float duration;
};

#endif // DISABLE_UNIT_TESTS
