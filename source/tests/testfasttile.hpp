#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/shadermanager.hpp"
#include "../performance/linegrap.hpp"
#pragma once

class Test_FastTiles: public State{
    public:
        Test_FastTiles(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 10.0f;
        };
        ~Test_FastTiles(){

        };
        void Begin(){
            btset = BufferTileMap(PointInt(32,32), PointInt3(25,16,2), Assets.make<Sprite>("data/tiles.png"));
            for (int l = 0; l<btset.GetMapSize().z; l++){
                for (int x = 0; x<btset.GetMapSize().x; x++){
                    for (int y = 0; y<btset.GetMapSize().y; y++){
                        btset.SetTile(PointInt3(x, y, l), Tile(rand()%150, rand()%4));
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

