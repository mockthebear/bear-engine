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
            shdr.Compile(GL_VERTEX_SHADER,"engine/vertex.glvs");
            bear::out << "Now frag\n";
            shdr.Compile(GL_FRAGMENT_SHADER,"engine/moq.glfs");
            shdr.Link();
            background = Sprite("data/wall.jpg");


            //ScreenManager::GetInstance().SetTopShader(m_shader);
        };

        void Update(float dt){
            shdr.Bind();
            duration -= dt;
            sinner += 3.14f * dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            shdr.SetUniform<float>("dt", 0.3f + fabs(sin(sinner))*0.2f );

        };
        void Render(){

            background.Render(Point(0,0),0);
            shdr.Unbind();
        };
        void Input();
        void Resume(){};
        void End(){
            m_shader.Close();
            //This dont necessary clear the shader data
            ScreenManager::GetInstance().DeleteTopShader();
        };
    private:
        Shader m_shader;
        float sinner;
        Shader shdr;
        Sprite background;
        float duration;
};


