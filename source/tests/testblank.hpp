#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#pragma once



class Test_: public State{
    public:
        Test_(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 10.0f;
        };
        ~Test_(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test");
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
        void End(){};
    private:
        float duration;
};

#endif // DISABLE_UNIT_TESTS
