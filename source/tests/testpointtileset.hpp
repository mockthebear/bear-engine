#pragma once
#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/pstileset.hpp"

class Test_PSTILESET: public State{
    public:
        Test_PSTILESET(){
            requestQuit = requestDelete = false;
            bear::out << "Test PSTILESET\n";
            duration = 100.0f;
        };
        ~Test_PSTILESET(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test PSTILESET");
            ps = new PointSpriteTileset();
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            ps->Render();
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
        PointSpriteTileset *ps;
};


