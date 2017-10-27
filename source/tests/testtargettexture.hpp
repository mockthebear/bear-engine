
#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#pragma once

class Test_TargetTexture: public State{
    public:
        Test_TargetTexture(){
            requestQuit = requestDelete = false;

            duration = 1000.0f;
        };
        ~Test_TargetTexture(){

        };
        void Begin(){
            bear::out << "Test target\n";
            background = Assets.make<Sprite>("data/wall.jpg");
            targ.Generate(400,400);
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                //requestDelete = true;
            }


        };
        void Render(){
            background.Render(0,0,0);
            targ.Bind();
            background.Render(0,0,0);


            glLoadIdentity();
            glTranslatef(4,400, 0.0f);

            glBegin( GL_QUADS );
                glColor4ub( 255,255,120,240 );
                glVertex2f( 0,0 );
                glVertex2f(  32, 0 );
                glVertex2f( 32,32 );
                glVertex2f( 0,  32 );
            glEnd();
            glPopMatrix();

            RenderHelp::DrawCircleColor(Point(400-64,400 - 64),32,255,0,255,255);
            targ.UnBind();
            targ.Render(Point(0,0));

            RenderHelp::DrawCircleColor(Point(400,400),200,0,0,255,155);


        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Sprite background;
        float duration;
        TargetTexture targ;
};

