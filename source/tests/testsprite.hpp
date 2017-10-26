#include "../settings/definitions.hpp"
#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../engine/sprite.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/shadermanager.hpp"

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

            /*
                Replace all WHITE with pure red without aliasing
            */
            bear::out << "Testing Load with color replacing\n";
            r.AddReplacer(RenderHelp::FormatRGBA(255,255,255,255),RenderHelp::FormatRGBA(255,0,0,255));
            Assets.load<Sprite>("data/raccoon.png","otherRaccoon",r,TEXTURE_TRILINEAR);


            bear::out << "Loading from alias\n";
            raccoonHead = Assets.make<Sprite>("someFancyName");
            cursor = Assets.make<Sprite>("otherRaccoon");
            sheet = Assets.make<Sprite>("data/totem.png");
            smol = Assets.make<Sprite>("data/doge death.png");

            sheet.SetGrid(32,64);
            sheet.SetFrame(0,0);
            sheet.SetFrameCount(4);
            sheet.SetFrameTime(0.2);
            /*
                Would give the same result.
                raccoonHead = Assets.make<Sprite>("someFancyName");
                raccoonHead = Assets.make<Sprite>("otherRaccoon");
                raccoonHead = Assets.make<Sprite>("data/raccoon.png");


            */

            tiles = Assets.make<Sprite>("data/tiles.png");
            tiles.SetGrid(32,32);

            bear::out << "Opening resources\n";
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }
            bear::out << "Load right from resources\n";
            background = Assets.make<Sprite>("test:wall.jpg");
            bearHead = Assets.make<Sprite>("test:bear.png",1,0,1,TEXTURE_TRILINEAR);
            bear::out << "Sprites loaded.\n";

            g_shader.Compile("engine/vertex.glvs","engine/color.glfs");



        };



        void Update(float dt){
            duration -= dt;
            sheet.Update(dt);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            g_shader.Bind();
            background.Render(0,0);
            g_shader.Unbind();
            RenderHelp::DrawSquareColor(10,10,SCREEN_SIZE_W-20,SCREEN_SIZE_H-20,255,0,255,255,true);
            bearHead.Render(Point(64,64),duration * 3.6f * 2.0f);
            raccoonHead.Render(Point(120,64),0);
            sheet.Render(200,200);
            for (int i=0;i<20;i++){
                tiles.SetFrame(i%12,0);
                tiles.Render(32*i,264);
                tiles.SetFrame((i+1)%12,0);
                tiles.Render(32*i,296);
            }
            smol.SetScale(Point(8,8));
            smol.Render(300,300,0);
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
        Sprite sheet;
        Sprite tiles;
        Sprite smol;
        float duration;
};




