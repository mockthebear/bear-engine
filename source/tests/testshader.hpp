#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/shadermanager.hpp"
#include "../engine/sprite.hpp"
#pragma once

class Test_Shader: public State{
    public:
        Test_Shader(){
            requestQuit = requestDelete = false;

            duration = 200.0f;
            sinner = 0.0f;
        };
        ~Test_Shader(){

        };
        void Begin(){
            bear::out << "Test shader\n";
            bear::out << "Compile an shader\n";
            shdr.Compile("engine/vertex.glvs","engine/moq.glfs");
            background = Sprite("data/wall.jpg");
        };

        void Update(float dt){
            duration -= dt;
            sinner += 3.14f * dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            shdr.Bind();

            shdr.SetUniform<float>("dt", 0.3f + fabs(sin(sinner))*0.2f );

            shdr.Unbind();

            g_shader.Bind();

            Point p = g_input.GetMouse();
            p.y = p.y/(float)SCREEN_SIZE_H;
            p.x = p.x/(float)SCREEN_SIZE_W;


            g_shader.SetUniform<Point>("Cent2d",p);
            g_shader.Unbind();


        };
        void Render(){


            shdr.Bind();
            //g_shader.Bind();
            background.Render(0,0,0);
            //g_shader.Unbind();
            shdr.Unbind();
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float sinner;
        Shader shdr;
        Sprite background;
        float duration;
};


