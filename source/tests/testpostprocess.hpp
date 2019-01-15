#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



#include "../engine/genericstate.hpp"
#include "../engine/screenmanager.hpp"
#include "../engine/shadermanager.hpp"
#include "../engine/sprite.hpp"
#pragma once

class Test_PostProcess: public State{
    public:
        Test_PostProcess(){
            requestQuit = requestDelete = false;
            bear::out << "Test post process\n";
            duration = 60.0f;
        };
        ~Test_PostProcess(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test post process");
            topShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
            topShader.Compile(GL_FRAGMENT_SHADER, "engine/invert.glfs");
            topShader.Link();
            ScreenManager::GetInstance().StartPostProcessing();
            ScreenManager::GetInstance().SetTopShader(topShader);


            bearHead = Assets.make<Sprite>("data/bear.png");
            background = Sprite("data/wall.jpg");

            mainShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
            mainShader.Compile(GL_FRAGMENT_SHADER, "engine/moq.glfs");
            mainShader.Link();

            example = Text("ui/UnB-Office_Regular.ttf",32,TEXT_SOLID,"Hello text",{33,33,120});
        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
                ScreenManager::GetInstance().StopPostProcessing();
            }
            mainShader.Bind();
            mainShader.SetUniform<float>("dt", 0.3f + fabs(sin(duration))*0.2f );
            mainShader.Unbind();
        };
        void Render(){
            mainShader.Bind();
            background.Render(Point(0,0));
            mainShader.Unbind();
            example.Render(Point(32,32));
            bearHead.Render(g_input.GetMouse(), duration * 10.0f );
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Shader m_shader;
        Shader topShader;
        Shader mainShader;
        Sprite background, bearHead;
        Text example;
        float duration;
};

#endif // RUN_TESTS
