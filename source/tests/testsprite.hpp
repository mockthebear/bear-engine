#include "../settings/definitions.hpp"
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

            sheet2 = Assets.make<Sprite>("data/totem.png",4,0.2);

            smol = Assets.make<Sprite>("data/doge death.png");
            smol.SetScale(Point(8,8));

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

            bear::out << "Opening resources\n";
            if (!ResourceManager::GetInstance().Load("test.burr","test")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }
            bear::out << "Load right from resources\n";
            background = Assets.make<Sprite>("test:wall.jpg");
            bearHead = Assets.make<Sprite>("test:bear.png",TEXTURE_TRILINEAR);
            bear::out << "Sprites loaded.\n";


            tset = TileMap(PointInt(32,32), PointInt3(16,4,2), Assets.make<Sprite>("data/tiles.png"));
            tset.SetBlankTile(1);
            btset = BufferTileMap(PointInt(32,32), PointInt3(16,4,2), Assets.make<Sprite>("data/tiles.png"));
            //btset.SetBlankTile(1);
            for (int l = 0; l<2;l++){
                for (int x = 0; x<16;x++){
                    for (int y = 0; y<4;y++){
                        tset.SetTile(PointInt3(x,y, l), Tile(y + x * 4 + l * 16 * 4, rand()%4));
                    }
                }
            }
            for (int l = 0; l<2;l++){
                for (int x = 0; x<16;x++){
                    for (int y = 0; y<4;y++){
                        btset.SetTile(PointInt3(x, y, l), Tile(rand()%100, rand()%4));
                    }
                }
            }

            btset.UpdateBuffers();
        };



        void Update(float dt){
            duration -= dt;
            sheet.Update(dt);
            sheet2.Update(dt);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                btset.SetTile(PointInt3(0, 0, 0), Tile(4, 0));
                btset.SetTile(PointInt3(0, 0, 1), Tile(127 + rand()%2, 1));
                btset.UpdateBuffers();
                //requestDelete = true;
            }

        };
        void Render(){


            background.Render(0,0,0);
            //
            bearHead.SetAlpha(120);
            bearHead.Render(Point(64,64),duration * 3.6f * 2.0f);
            raccoonHead.SetScale(Point(2.0f,1.0f));
            raccoonHead.Render(Point(120,64),0);
            sheet.Render(200,200);
            sheet2.Render(232,200);

            smol.Render(300,300,0);
            cursor.Render(g_input.GetMouse());

            tset.Render(Point(32,320));
            btset.Render(Point(32,0));

            RenderHelp::DrawSquareColor(Rect(10,10,SCREEN_SIZE_W-20,SCREEN_SIZE_H-20),255,0,255,255,true);
            RenderHelp::DrawCircleColor(Point(400,400),86,255,0,100,100);

        };
        void Input();
        void Resume(){};
        void End(){
            Assets.erase();
            ResourceManager::GetInstance().Erase("test");
        };
    private:
        TileMap tset;
        BufferTileMap btset;
        Sprite background;
        Sprite bearHead;
        Sprite raccoonHead;
        Sprite cursor;
        Sprite sheet,sheet2;
        Sprite smol;
        float duration;
};




