#include "title.hpp"
#include <sstream>
#include <cmath>

#include "title.hpp"

#include "../luasystem/luacaller.hpp"
#include "../luasystem/luatools.hpp"
#include "../engine/renderhelp.hpp"


#include "../tests/testthread.hpp"
#include "../tests/testfiles.hpp"


Title::Title(){
    requestQuit = requestDelete = false;
    /*
        Loading main lua script
    */
    #ifndef __EMSCRIPTEN__
    LuaCaller::LoadFile(LuaManager::L,"lua/test.lua");
    LuaCaller::Pcall(LuaManager::L);
    #endif // __EMSCRIPTEN__

    /*
        Starting camera
    */

    Camera::Initiate(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),128,200);
    Camera::speed = 0;

}


void Title::Begin(){
    /*
        Callin an lua function
    */
    #ifndef __EMSCRIPTEN__
    LuaCaller::CallGlobalField(LuaManager::L,"onLoad");
    #endif // __EMSCRIPTEN__

    PointInt3 size(26,26,3);
    region = BearPolygon();
    btset = BufferTileMap(PointInt(32,32), size, Assets.make<Sprite>("data/tiles.png"));

    //btset.SetTile(PointInt3(0, 0, 0), Tile(4));
    //{0, 0}, {10, 0}, {10, 10}, {0, 10}
    Point center(360,280);

    for (int i=0;i<15;i++){
        float angle = Geometry::toRad( 360 *  i/ 15.0f );
        float rad =  100 + (rand()%200);
        region.Add(Point(center.x + rad * sin(angle),center.y  + rad * cos(angle)));
    }

    std::vector<Point> inside;

    //region.Add(Point(150,150));
    //region.Add(Point(100,200));

    for (int x = 0; x<size.x; x++){
        for (int y = 0; y<size.y; y++){
            Point kd(x * 32, y * 32);
            if (region.IsInside(kd)){
                inside.emplace_back(Point(x, y));
                PointInt3 tilePos(x, y, 0);
                btset.SetTile(tilePos, Tile(4));

                tilePos.z = 1;
                Tile &t2 = btset.GetTile(tilePos);
                t2.id = 0;
                tilePos.z = 2;
                Tile &t3 = btset.GetTile(tilePos);
                t3.id = 0;

                tilePos.z = 0;

                for (int x=-1;x<2;x++){
                    for (int y=-1;y<2;y++){
                        if (x == 0 && y == 0)
                            continue;
                        PointInt3 auxPos = tilePos;
                        auxPos.y += y;
                        auxPos.x += x;
                        if (btset.IsValidPosition(auxPos)){
                            BorderTile(auxPos, true);
                            BorderTile(auxPos, false);
                        }
                    }
                }

            }
        }
    }

    for (auto &it : inside){
        if (rand()%1000 < 100){
            PointInt3 tilePos(it.x, it.y, 1);

            int sel = rand()%(5 + (btset.GetTile(PointInt3(it.x, it.y-1, 1)).id == 0 ? 3 : 0 )  );
            if (sel == 0){
                btset.SetTile(tilePos, Tile(118));
            }else if (sel == 1){
                btset.SetTile(tilePos, Tile(128));
            }else if (sel == 2){
                btset.SetTile(tilePos, Tile(127));
            }else if (sel == 3){
                btset.SetTile(tilePos, Tile(27));
            }else if (sel == 4){
                btset.SetTile(tilePos, Tile(28));
            }else if (sel == 5){
                btset.SetTile(tilePos, Tile(69));
                tilePos.y -= 1;
                btset.SetTile(tilePos, Tile(29));
            }else if (sel == 6){
                btset.SetTile(tilePos, Tile(129));
                tilePos.y -= 1;
                btset.SetTile(tilePos, Tile(117));
            }else if (sel == 7){
                btset.SetTile(tilePos, Tile(129));
                tilePos.y -= 1;
                btset.SetTile(tilePos, Tile(119));
            }


        }
    }

    btset.UpdateBuffers();



}

Title::~Title(){
    Pool.ErasePools();
}


void Title::BorderTile(PointInt3 pos, bool sides){
    uint32_t tidBase = 0;
    uint32_t edgeBase = 0;

    int checkMatrix[3][3] = {
        {0   ,   64   ,0   },
        {16  ,   0    ,8   },
        {0   ,   2    ,0   },
    };

    int edgeMatrix[3][3] = {
        {1  ,   128    ,2   },
        {128  ,   0    ,128   },
        {4  ,   128    ,8   },
    };

    PointInt3 auxPos2 = pos;

    for (int x=-1;x<2;x++){
        for (int y=-1;y<2;y++){
            PointInt3 auxPos = pos;
            auxPos.y += y;
            auxPos.x += x;
            if (btset.IsValidPosition(auxPos)){
                Tile &t = btset.GetTile(auxPos);
                if (t.id == 4){
                    tidBase += checkMatrix[y+1][x+1];
                    edgeBase += edgeMatrix[y+1][x+1];
                }
            }
        }
    }

    std::map<int,int> borderMap;
    borderMap[1] = 12;
    borderMap[2] = 24;
    borderMap[4] = 13;
    borderMap[8] = 15;
    borderMap[16] = 25;
    borderMap[32] = 22;
    borderMap[64] = 14;
    borderMap[128] = 23;


    borderMap[7] = 24;
    borderMap[224] = 14;
    borderMap[41] = 15;
    borderMap[148] = 25;
    borderMap[6] = 24;
    borderMap[3] = 24;
    borderMap[192] = 14;
    borderMap[96] = 14;
    borderMap[40] = 15;
    borderMap[9] = 15;
    borderMap[144] = 25;
    borderMap[20] = 25;

    borderMap[80] = 10;
    borderMap[10] = 21;
    borderMap[18] = 20;
    borderMap[72] = 11;

    borderMap[66] = 7;
    borderMap[24] = 18;

    borderMap[88] = 19;
    borderMap[26] = 9;

    borderMap[82] = 5;
    borderMap[74] = 8;
    borderMap[90] = 26;

    std::map<int,int> edgeMap;

    edgeMap[1] = 23;
    edgeMap[2] = 22;
    edgeMap[4] = 13;
    edgeMap[8] = 12;

    edgeMap[9] = 193;
    edgeMap[6] = 194;

    edgeMap[3] = 195;
    edgeMap[12] = 192;
    edgeMap[10] = 203;
    edgeMap[5] = 204;

    edgeMap[15] = 191;
    edgeMap[7]  = 201;
    edgeMap[14] = 211;
    edgeMap[11] = 212;
    edgeMap[13] = 213;

    if (sides){
        if (tidBase != 0){
            if (borderMap.find(tidBase) != borderMap.end()){
                if (btset.GetTile(pos).id == 0){
                    pos.z = 1;
                    Tile &t = btset.GetTile(pos);
                    t.id = borderMap[tidBase];
                }
            }else{
                std::cout << "Unknow combination " << tidBase << "\n";
            }
        }
    }else{
        if (edgeBase > 0){
            if (edgeBase >= 128){
                pos.z = 2;
                Tile &t = btset.GetTile(pos);
                t.id = 0;
            }
            if (edgeMap.find(edgeBase) != edgeMap.end()){
                if (btset.GetTile(pos).id == 0){
                    pos.z = 2;
                    Tile &t = btset.GetTile(pos);
                    t.id = edgeMap[edgeBase];
                }
            }else{
                std::cout << "Unknow edge " << edgeBase << "\n";
            }
        }
    }

}

void Title::Update(float dt){
    /*
        Reset
    */
    if( g_input.KeyPress(SDLK_t) ){
        requestDelete = true;
        return;
    }



    if (g_input.KeyPress(SDLK_n)){
        Begin();
    }
    if (g_input.MousePress(1)){
        PointInt mPos = g_input.GetMouse();
        mPos.x = mPos.x / 32;
        mPos.y = mPos.y / 32;

        PointInt3 tilePos(mPos.x, mPos.y, 0);

        if (btset.IsValidPosition(tilePos)){
            btset.SetTile(tilePos, Tile(4));

            tilePos.z = 1;
            Tile &t2 = btset.GetTile(tilePos);
            t2.id = 0;
            tilePos.z = 2;
            Tile &t3 = btset.GetTile(tilePos);
            t3.id = 0;

            tilePos.z = 0;

            for (int x=-1;x<2;x++){
                for (int y=-1;y<2;y++){
                    if (x == 0 && y == 0)
                        continue;
                    PointInt3 auxPos = tilePos;
                    auxPos.y += y;
                    auxPos.x += x;
                    if (btset.IsValidPosition(auxPos)){
                        BorderTile(auxPos, true);
                        BorderTile(auxPos, false);
                    }
                }
            }

            btset.UpdateBuffers();
        }
    }


    Input();

    Pool.Update(dt);
    Map.clear();
    Pool.PreRender(Map);


    Camera::Update(dt);

    UpdateWindowses(dt);


}

void Title::Render(){
    RenderHelp::DrawSquareColor(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H,255,255,255,255);


    RenderInstances();

    RenderWindowses();

    btset.Render(Point(0,0));

    Console::GetInstance().Render();

    std::vector<GameVertice> lines;
    Point OldP(0, 0);
    Point OldF(0, 0);
    bool first = true;
    for (auto &it : region.vertices){
        if (first){
            OldP.x = it.x;
            OldP.y = it.y;
            OldF.x = it.x;
            OldF.y = it.y;
            first = false;
        }
        lines.emplace_back(GameVertice(OldP.x        ,   OldP.y       , 1.0f, 1.0f, 1.0f, 1.0f));
        lines.emplace_back(GameVertice(it.x        ,   it.y       , 1.0f, 1.0f, 1.0f, 1.0f));
        OldP.x = it.x;
        OldP.y = it.y;
    }
    lines.emplace_back(GameVertice(OldP.x        ,   OldP.y       , 1.0f, 1.0f, 1.0f, 1.0f));
    lines.emplace_back(GameVertice(OldF.x        ,   OldF.y       , 1.0f, 1.0f, 1.0f, 1.0f));


    RenderHelp::DrawCircleColor(g_input.GetMouse(), 16, region.IsInside(g_input.GetMouse()) ? 255 : 0, 100,100, 150 );

    RenderHelp::DrawLinesColor(lines,0,0,0,255,2);




}

void Title::Input(){
    if (InputManager::GetInstance().ShouldQuit()){
        Console::GetInstance().AddText("DEVE QITAR");
        requestQuit = true;
    }


    if( InputManager::GetInstance().KeyPress(SDLK_ESCAPE) ) {
        Console::GetInstance().AddText("SCAPE");
        requestDelete = true;
    }
}
