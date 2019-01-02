#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../framework/segment/segmenter.hpp"
#pragma once

class DummyNode: public SegmentNode{
    public:
        DummyNode():doin(false){};

        void Start(Segdata *owner, PointInt location){
            SegmentNode::Start(owner, location);
            color[0] = rand()%255;
            color[1] = rand()%255;
            color[2] = rand()%255;
            for (int i=0;i<10;i++){
                AddPoint(Point(m_nodeSize.x*m_location.x  + m_offset.x + rand()%m_nodeSize.x, m_nodeSize.y*m_location.y + m_offset.y + rand()%m_nodeSize.y));
            }
        };

        void Update(float dt){
            int moveSize = 20;
            for (auto &it : points){
                it.x += ( (float)(rand()%(moveSize*2 +1)) -moveSize)*dt;
                it.y += ( (float)(rand()%(moveSize*2 +1)) -moveSize)*dt;
            }
        }

        /**
            Must contain a
            virtual bool Call(uint32_t mode, ... your arguments here) = 0;
        */
        bool Call(uint32_t mode, int owo){
            //bear::out << "Called: " << m_location << " ["<<m_id<<"] with mode "<<mode<<"\n";
            RenderHelp::DrawSquareColor(GetBox(), owo,255,255,80, !doin);
            RenderHelp::DrawPointsColor(points,color[0],color[1],color[2],255);

            return true;
        }

        void AddPoint(Point p){
            points.emplace_back(p);
        }


        template <int SizeX> void Deactivate(DummyNode seg[][SizeX]){
            Rect myArea = GetBox();
            doin = false;

            for (auto it = points.begin(); it != points.end();){
                if (!myArea.IsInside(*it)){
                    PointInt locate = GetCoordinateByPosition(*it);
                    if (InBounds(locate) && GetId() != seg[locate.y][locate.x].GetId() ){
                        seg[locate.y][locate.x].AddPoint(*it);
                    }
                    it = points.erase(it);
                }else{
                    ++it;
                }
            }
        };
        template <int SizeX> void Activate(DummyNode seg[][SizeX]){
            doin = true;
        };


    private:
        uint8_t color[3];
        std::vector<Point> points;
        bool doin;
};





class Test_Node: public State{
    public:
        Test_Node(){
            requestQuit = requestDelete = false;
            bear::out << "Test node\n";
            duration = 1000.0f;
        };
        ~Test_Node(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test node");
            segm = new Segmenter<DummyNode, 12, 12>(PointInt(32,32), Point(100,100));
        };

        void Update(float dt){
            duration -= dt;
            affectedArea = Rect(g_input.GetMouse().x - 50, g_input.GetMouse().y -50, 100, 100);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 100.0f ) {
                requestDelete = true;

            }
            segm->UpdateActives(affectedArea);
            segm->Update(dt);




        };
        void Render(){
            segm->Call((uint32_t)1, false, 120 + rand()%127);
            RenderHelp::DrawSquareColor(affectedArea,255,255,255,100);
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
        Rect affectedArea;
        Segmenter<DummyNode, 12, 12> *segm;
};

