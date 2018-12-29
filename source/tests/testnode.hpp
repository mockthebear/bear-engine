#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#include "../framework/segment/segmenter.hpp"
#pragma once

class DummyNode: public SegmentNode{
    public:
        DummyNode():doin(false),m_location(0,0){};

        void Start(int32_t id, PointInt size, PointInt location){
            m_location = location;
            m_id = id;
            m_size = size;
            bear::out << "[Hello node] "<<id<<" at " << m_location << "\n";
        };

        void Update(float dt){
            bear::out << "Updating: " << m_location << " ["<<m_id<<"]\n";
        }

        /**
            Must contain a
            virtual bool Call(uint32_t mode, ... your arguments here) = 0;
        */
        bool Call(uint32_t mode, int owo){
            //bear::out << "Called: " << m_location << " ["<<m_id<<"] with mode "<<mode<<"\n";
            RenderHelp::DrawSquareColor(Rect(m_location.x * m_size.x, m_location.y * m_size.y, m_size.x, m_size.y), owo,255,255,255, !doin);
            return true;
        }



        void Deactivate(){
            bear::out << "Deactivated: " << m_location << " ["<<m_id<<"]\n";
            doin = false;
        };
        void Activate(){
            bear::out << "Activated: " << m_location << " ["<<m_id<<"]\n";
            doin = true;
        };

        PointInt GetSize(){return m_size;};
    private:
        bool doin;
        PointInt m_location;

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

            segm = new Segmenter<DummyNode, 12, 12>(PointInt(32,32));
        };

        void Update(float dt){
            duration -= dt;
            affectedArea = Rect(g_input.GetMouse().x - 50, g_input.GetMouse().y -50, 100, 100);
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1  ) {
                //requestDelete = true;
                segm->UpdateActives(affectedArea);
            }




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

