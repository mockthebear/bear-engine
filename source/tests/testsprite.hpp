#include "../settings/definitions.hpp"
#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../engine/sprite.hpp"

#include "../framework/resourcemanager.hpp"


class Test_Sprite: public State{
    public:
        Test_Sprite(){
            requestQuit = requestDelete = false;
            duration = 50.0f;

        };
        ~Test_Sprite(){

        };
        void Begin(){
            bear::out << "Testing sprites\n";
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
            }
            background = Assets.make<Sprite>("test:wall.jpg");
            bearHead = Assets.make<Sprite>("test:bear.png");
            bear::out << "Sprites loaded.\n";




        };



        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            background.Render(0,0);
            bearHead.Render(Point(64,64),duration * 3.6f * 2.0f);
        };
        void Input();
        void Resume(){};
        void End(){
            ResourceManager::GetInstance().Erase("test");
        };
    private:
        Sprite background;
        Sprite bearHead;
        float duration;
};




