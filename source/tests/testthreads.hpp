#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"

#pragma once



class Test_Thread: public State{
    public:
        Test_Thread(){
            requestQuit = requestDelete = false;
            bear::out << "Test thread\n";
            duration = 10000.0f;
            perc = 1.0f;
        };
        ~Test_Thread(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test threads");
            bear::out << "Starting four threads\n";

            info = Text("Test info: press T to add a heavy job. Press R to add a visual heavy job.",19,{255, 255, 220});
            fpsInfo = Text("FPS: 000",24,{255, 255, 220});
        };

        void End(){
            bear::out << "Terminating threads\n";

            bear::out << "Done\n";
        };

        void Update(float dt){
            duration -= dt;
            /**/
            if (g_input.KeyPress(SDLK_t)){

            }else if (g_input.KeyPress(SDLK_r)){

            }else if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            fpsInfo.SetText(utils::format("FPS %3f", ScreenManager::GetInstance().GetFps()));

        };
        void Render(){

        };
        void Input();
        void Resume(){};

    private:
        Text info, fpsInfo;
        float perc;
        float duration;
};

#endif // DISABLE_UNIT_TESTS

