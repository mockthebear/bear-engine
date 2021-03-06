#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../engine/sprite.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/shadermanager.hpp"
#include "../engine/tiles/tilemap.hpp"
#include "../engine/tiles/buffertilemap.hpp"

#include "../framework/resourcemanager.hpp"


class Test_Sprite: public State{
    public:
        Test_Sprite():State(){

            requestQuit = requestDelete = false;
            duration = 600.0f;

        };
        ~Test_Sprite(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test sprite");
            bear::out << "Testing Load sprite from an direct file:\n";

            //Loading the same sprite twice, and seting an nickname and use anti aliasing
            Assets.load<Sprite>("data/raccoon.png","someFancyName");
            ColorReplacer r;


            bear::out << "Testing Load with color replacing\n";
            r.AddReplacer(RenderHelp::FormatRGBA(255,255,255,255),RenderHelp::FormatRGBA(255,0,0,255));
            Assets.load<Sprite>("data/raccoon.png","otherRaccoon",r,TEXTURE_TRILINEAR);


            bear::out << "Loading from alias\n";
            raccoonHead = Assets.make<Sprite>("someFancyName");
            cursor = Assets.make<Sprite>("otherRaccoon");


            sheet2 = AnimatedSprite(Assets.make<Sprite>("data/totem.png"));

            sheet2.SetGridSize(32,64);
            sheet2.SetFrame(0,2);
            sheet2.SetFrameCount(4);
            sheet2.SetFrameTime(0.2);

            smol = Assets.make<Sprite>("data/doge death.png");
            smol.SetScale(Point(8,8));

            //sheet.SetGridSize(32,64);

            sheet = Assets.make<AnimatedSprite>("data/totem.png");
            sheet.LoadAnimationScript("data/test.anim");
            sheet.RunAnimationSegment("SETUP");
            sheet.AddCallback("check",std::function<bool()>([](){
                int r = rand()%100;
                std::cout << "Attack is: "<<(r<20) << "\n";
                return (r<20);
            }));

            sheet.AddCallback("oi",std::function<bool()>( [](){
                std::cout << "ATTACKED\n";
                return true;
            }));
            /*
                Would give the same result.
                raccoonHead = Assets.make<Sprite>("someFancyName");
                raccoonHead = Assets.make<Sprite>("otherRaccoon");
                raccoonHead = Assets.make<Sprite>("data/raccoon.png");
            */

            failedSprite = Assets.make<Sprite>("nofile.png");

            bear::out << "Opening resources\n";
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }
            bear::out << "Load right from resources\n";
            background = Assets.make<Sprite>("test:wall.jpg");
            bearHead = Assets.make<Sprite>("test:bear.png",TEXTURE_TRILINEAR);
            bear::out << "Sprites loaded.\n";


        };



        void Update(float dt){
            duration -= dt;
            sheet.Update(dt);
            sheet2.Update(dt);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){

            background.Render(PointInt(0,0));


            bearHead.SetAlpha(120);

            bearHead.SetRotation(duration * 3.6f * 2.0f);
            bearHead.Render(Point(64,64));

            raccoonHead.BeginRender();
                raccoonHead.SetScale(Point(2.0f,1.0f));
                raccoonHead.Render(Point(120,64));

                raccoonHead.SetScale(Point(2.0f,2.0f));
                raccoonHead.Render(Point(120,80));

                raccoonHead.SetScale(Point(0.5f,0.5f));
                raccoonHead.Render(Point(120,330));
            raccoonHead.EndRender();

            sheet.Render(PointInt(300,100) );
            sheet2.Render(PointInt(332,100));

            smol.Render({300,300});
            cursor.SetRotation(-duration * 3.6f * 2.0f);
            cursor.Render(g_input.GetMouse() - cursor.GetSize()/2);



            RenderHelp::DrawSquareColor(Rect(10,10,SCREEN_SIZE_W-20,SCREEN_SIZE_H-20),255,0,255,255,true);
            RenderHelp::DrawCircleColor(CircleColor(Circle(400,400,86),BearColor(255,0,100,100)) );




            failedSprite.Render(Point(200,100));

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
        AnimatedSprite sheet,sheet2;
        Sprite smol;
        Sprite failedSprite;
        float duration;
};



#endif // DISABLE_UNIT_TESTS
