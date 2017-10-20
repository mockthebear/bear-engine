#include "../settings/definitions.hpp"
#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../engine/sprite.hpp"
#include "../engine/renderhelp.hpp"

#include "../framework/resourcemanager.hpp"


class Test_Sprite: public State{
    public:
        Test_Sprite():State(){

            requestQuit = requestDelete = false;
            duration = 50.0f;

        };
        ~Test_Sprite(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test sprite");
            bear::out << "Testing Load sprite from an direct file:\n";

            //Loading the same sprite twice, and seting an nickname and use anti aliasing
            Assets.load<Sprite>("data/raccoon.png","someFancyName",true);
            ColorReplacer r;

            /*
                Replace all WHITE with pure red without aliasing
            */
            r.AddReplacer(RenderHelp::FormatRGBA(255,255,255,255),RenderHelp::FormatRGBA(255,0,0,255));
            Assets.load<Sprite>("data/raccoon.png","otherRaccoon",r);



            raccoonHead = Assets.make<Sprite>("someFancyName");
            cursor = Assets.make<Sprite>("otherRaccoon");
            /*
                Would give the same result.
                raccoonHead = Assets.make<Sprite>("someFancyName");
                raccoonHead = Assets.make<Sprite>("otherRaccoon");
                raccoonHead = Assets.make<Sprite>("data/raccoon.png");


            */

            bear::out << "Opening resources\n";
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }
            bear::out << "Load right from resources\n";
            background = Assets.make<Sprite>("test:wall.jpg");
            bearHead = Assets.make<Sprite>("test:bear.png");
            bear::out << "Sprites loaded.\n";



        };



        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            background.Render(0,0);
            bearHead.Render(Point(64,64),duration * 3.6f * 2.0f);
            raccoonHead.Render(Point(120,64),0);
            cursor.Render(g_input.GetMouse());
        };
        void Input();
        void Resume(){};
        void End(){
            Assets.erase();
            ResourceManager::GetInstance().Erase("test");
        };
    private:
        Sprite background;
        Sprite bearHead;
        Sprite raccoonHead;
        Sprite cursor;
        float duration;
};




