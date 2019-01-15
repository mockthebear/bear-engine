#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



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
            ScreenManager::GetInstance().SetScreenName("Test shader");
            bear::out << "Test shader\n";
            bear::out << "Compile an shader\n";
            shdr.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
            shdr.Compile(GL_FRAGMENT_SHADER, "engine/moq.glfs");
            shdr.Link();

            shdr2.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
            shdr2.Compile(GL_FRAGMENT_SHADER, "engine/merger.glfs");
            shdr2.Link();
            background = Sprite("data/wall.jpg");
            bear = Sprite("data/bear.png");

            bear::out << "creating noise\n";

            unsigned char pixels[64*64];
            for (uint32_t i = 0; i<64*64;i++){
                uint8_t col = rand()%255;
                pixels[i] = col;
            }
            TextureLoadMethod meth(TEXTURE_NEAREST);
            bear::out << "creating texture noise\n";
            noise = Painter::MakeTexture(PointInt(64,64),GL_ALPHA,(unsigned char*)pixels,meth);


        };

        void Update(float dt){
            duration -= dt;
            sinner += 3.14f * dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }
        };
        void Render(){


            shdr.Bind();
            shdr.SetUniform<float>("dt", 0.3f + fabs(sin(sinner))*0.2f );
            background.Render(0,0,0);
            shdr.Unbind();

            shdr2.Bind();
            ShaderSetter<BearTexture>::SetUniform(Shader::GetCurrentShaderId(),"image2",(*noise),1);
            ShaderSetter<float>::SetUniform(Shader::GetCurrentShaderId(),"offsetNoise",sinner/31.4f);
            bear.Render(PointInt(120,120),0);
            ShaderSetter<TexturePtr>::DisableTexture(1);
            shdr2.Unbind();

            RenderHelp::DrawSquareColor(Rect(32,32,64,64),255,255,100,255);

        };
        void Input();
        void Resume(){};
        void End(){
            delete noise;

            //This dont necessary clear the shader data
        };
    private:
        float sinner;
        Shader shdr;
        Shader shdr2;
        Sprite background,bear;
        BearTexture* noise;
        float duration;
};


#endif // DISABLE_UNIT_TESTS
