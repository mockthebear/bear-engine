#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../framework/segment/segmenter.hpp"
#include "../engine/tiles/nodetileset.hpp"
#pragma once



#define NodeCount 6

#define TileUnitSize 32
#define NodeSize 2 * TileUnitSize

class Test_NodeTileset: public State{
    public:
        Test_NodeTileset(){
            requestQuit = requestDelete = false;
            bear::out << "Test node\n";
            duration = 1000.0f;
        };
        ~Test_NodeTileset(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test node tileset");

            Sprite spr = Assets.make<Sprite>("data/tiles.png");

            GameFile fmap("data/tilemap.txt");
            PointInt3 size;
            size.x = fmap.GetNumber(true);
            size.y = fmap.GetNumber(true);
            size.z = fmap.GetNumber(true);

            PointInt nodeSize(NodeSize, NodeSize);
            PointInt tileSizes(TileUnitSize,TileUnitSize);
            Point offset(32,32);

            segm = new SegmentTileset<NodeCount, NodeCount>(nodeSize, tileSizes, size.y, spr, offset);


            for (int l = 0; l<size.z; l++){
                for (int y = 0; y<size.y; y++){
                    for (int x = 0; x<size.x; x++){
                        segm->CallByPos(1, Point(x * 32 + offset.x, y * 32 + offset.y), PointInt3(x, y, l), Tile(fmap.GetNumber(true)) );
                    }
                }
            }
            segm->Call(2, false);
        };

        void Update(float dt){
            duration -= dt;
            affectedArea = Rect(g_input.GetMouse().x - 50, g_input.GetMouse().y -50, 100, 100);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 100.0f ) {
                requestDelete = true;

            }
            segm->UpdateActives(affectedArea);
        };

        void Render(){
            segm->Call((uint32_t)0, true);
            segm->Call((uint32_t)3, false);
            RenderHelp::DrawSquareColor(affectedArea,255,255,255,50);
        };

        void Input();
        void Resume(){};
        void End(){};

    private:
        float duration;
        Rect affectedArea;
        SegmentTileset<NodeCount, NodeCount> *segm;
};

#undef TileUnitSize
#undef NodeSize
#undef NodeCount


