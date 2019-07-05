#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../engine/spriteatlas.hpp"
#pragma once



class Test_Atlas: public State{
    public:
        Test_Atlas(){
            requestQuit = requestDelete = false;
            bear::out << "Test sprite atlas\n";
            duration = 10000.0f;
            m_atlas.Generate(PointInt(512,512));


            Sprite aux;
            Sprite aux2;
            Sprite aux3;
            Sprite aux4;
            Sprite aux5;
            Sprite aux6;

            aux.Open("data/ball lightning1.png");
            aux2.Open("data/arc.png");
            aux3.Open("data/totem.png");
            aux4.Open("data/bear.png");
            aux5.Open("data/raio.png");
            aux6.Open("data/gosma besouro chao.png");
            for (int i=0;i<10;i++){
                m_atlas.StichSprite(aux2);
            }

            m_atlas.StichSprite(aux3);
            for (int i=0;i<20;i++){
                m_atlas.StichSprite(aux2);
            }

            m_atlas.StichSprite(aux6);

            m_atlas.StichSprite(aux4);


            m_atlas.StichSprite(aux5);


            m_atlas.StichSprite(aux2);


            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);
            m_atlas.StichSprite(aux4);



            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);
            m_atlas.StichSprite(aux5);


            spr = m_atlas.SummonSprite(0);


        };
        ~Test_Atlas(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test sprite atlas");
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                //requestDelete = true;
            }

        };
        void Render(){

            spr.SetClip(0,0,16,16);
            spr.SetRotation(45);
            spr.Render(PointInt(32,32));

            spr.Render(PointInt(64,64));

            m_atlas.RenderAll();
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Sprite spr;
        SpriteAtlas m_atlas;
        float duration;
};

#endif // DISABLE_UNIT_TESTS

