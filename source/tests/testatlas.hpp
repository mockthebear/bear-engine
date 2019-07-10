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
            duration = 1000.0f;
            m_atlas.Generate(PointInt(512,512));


            uint32_t arcc = m_atlas.StichSprite("data/arc.png");
            uint32_t BLight = m_atlas.StichSprite("data/ball lightning1.png");
            uint32_t totemId = m_atlas.StichSprite("data/totem.png");
            m_atlas.StichSprite("data/bear.png");
            m_atlas.StichSprite("data/raio.png");
            m_atlas.StichSprite("data/gosma besouro chao.png");
            uint32_t tileId =m_atlas.StichSprite("data/super fire.png");


            no_a_arc = m_atlas.SummonSprite(BLight);

            raio = m_atlas.SummonSprite(BLight);
            raio.SetAnimation(0, 4, 1.0f);
            raio.SetGridSize(32,32);

            raio2 = AnimatedSprite(m_atlas.SummonSprite(BLight));
            raio2.SetAnimation(0, 4, 1.0f);
            raio2.SetGridSize(32,32);


            totem = AnimatedSprite(m_atlas.SummonSprite(totemId));
            totem.SetGridSize(32,64);
            totem.SetFrame(0,0);
            totem.SetFrameCount(4);
            totem.SetFrameTime(1.0);

            tile = m_atlas.SummonSprite(tileId);


            arc = m_atlas.SummonSprite(arcc);







        };
        ~Test_Atlas(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test sprite atlas");
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }
            raio.Update(dt);
            raio2.Update(dt);
            totem.Update(dt);

        };
        void Render(){

            /*arc.SetClip(0,0,16,16);
            arc.SetRotation(45);
            arc.Render(PointInt(32,32));

            arc.SetRotation(0);
            arc.SetClip(0,0,32,32);
            arc.Render(PointInt(64,64));*/
            for (int x=0;x<8;++x)
                for (int y=0;y<8;++y)
                    tile.Render(PointInt(x,y)*32);

            raio.Render(PointInt(64,64));
            no_a_arc.Render(PointInt(64,200));
            raio2.Render(PointInt(64,232));

            arc.SetRotation(0);
            arc.SetClip(0,0,32,32);
            arc.Render(PointInt(32,64));

            totem.Render({0,0});



            m_atlas.RenderAll();

            m_atlas.RenderWholeAtlas(PointInt(200,0));
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Sprite arc;
        Sprite tile;
        Sprite no_a_arc;
        AnimatedSprite raio, raio2, totem;
        SpriteAtlas m_atlas;
        float duration;
};

#endif // DISABLE_UNIT_TESTS

