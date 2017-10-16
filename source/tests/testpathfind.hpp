#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#pragma once

#include "../engine/pathfind.hpp"
#include "../engine/renderhelp.hpp"

class Test_PathFind: public State{
    public:
        Test_PathFind(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 80.0f;
            start = Point(100,32);
            finish = Point(550,550);
            pf = nullptr;
        };
        ~Test_PathFind(){

        };
        void Begin(){
            pf = new PathFind(600,600,4,16);
            for (int i=0;i<300;i++){
                Point p(rand()%600,rand()%600);
                if (pf->AddBlock(p.x,p.y,true)){
                    Blocks.emplace_back(p);
                }

            }

            std::stack<Point> auxWay = pf->Find(start,finish);
            while (auxWay.size() > 0){
                Path.emplace_back(auxWay.top());
                auxWay.pop();
            }
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            for (auto &it : Blocks){
                int x = ((int)it.x)%16;
                int y = ((int)it.y)%16;
                RenderHelp::DrawSquareColor(it.x-x,it.y-y,16,16,255,255,255,255);
            }
            Point first = finish;
            int iter = 0;
            for (auto &it : Path){
                RenderHelp::DrawLineColorA(first.x+8,first.y+8,it.x+8,it.y+8,255,(iter/(float)Path.size())*255,0,255);
                first = it;
                iter++;
            }
        };
        void Input();
        void Resume(){};
        void End(){
            if (pf)
                pf->Close();
        };
    private:
        std::vector<Point> Path;
        std::vector<Point> Blocks;
        float duration;
        PathFind* pf;
        Point start,finish;
};


