#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



#include "../engine/genericstate.hpp"
#include "../engine/shadermanager.hpp"
#include "../framework/gamefile.hpp"
#include "../performance/linegrap.hpp"
#pragma once


class Test_FastTiles: public State{
    public:
        Test_FastTiles(){
            requestQuit = requestDelete = false;
            bear::out << "Test Test_FastTiles\n";
            duration = 10.0f;
        };
        ~Test_FastTiles(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test static tileset");
            GameFile fmap("data/tilemap.txt");
            PointInt3 size;
            size.x = fmap.GetNumber(true);
            size.y = fmap.GetNumber(true);
            size.z = fmap.GetNumber(true);

            bear::out << "Tile size is: " << size << "\n";

            btset = BufferTileMap(PointInt(32,32), size, Assets.make<Sprite>("data/tiles.png"));


            for (int l = 0; l<btset.GetMapSize().z; l++){
                for (int y = 0; y<btset.GetMapSize().y; y++){
                    for (int x = 0; x<btset.GetMapSize().x; x++){
                        btset.SetTile(PointInt3(x, y, l), Tile(fmap.GetNumber(true)) );
                    }
                }
            }
            btset.UpdateBuffers();
        };

        void Update(float dt){
            duration -= dt;
            if(duration <= 0 ) {

            }
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1){
                requestDelete = true;
            }

        };
        void Render(){
            btset.Render(Point(32,32));
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        BufferTileMap btset;
        float duration;
};

#endif // RUN_TESTS
