#include "../settings/definitions.hpp"
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
            bgm = Sound("fogurai.ogg");
            //bgm.Play(1);
            //getchar();
            //bgm.Kill();
            Assets.erase();

        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){

        };
        void Input();
        void Resume(){};
        void End(){
            bear::out << "Rip sound\n";

        };
    private:
        Sound bgm;
        float duration;
};


