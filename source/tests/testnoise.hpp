#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../framework/perlinnoise.hpp"
#include "../engine/shadermanager.hpp"
#pragma once



class Test_Perlin: public State{
    public:
        Test_Perlin(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 1000000.0f;
        };
        ~Test_Perlin(){

        };
        void Begin(){

            shdr.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
            shdr.Compile(GL_FRAGMENT_SHADER, "engine/noiser.glfs");
            shdr.Link();

            sp2 = Sprite("engine/churras.jpg");

            float frequency = 2.0f;
            ScreenManager::GetInstance().SetScreenName("Test");
            const siv::PerlinNoise perlin(123);

            constexpr int sx = 600;
            constexpr int sy = 338;
            uint32_t octaves = 4;

            const double fx = sx / frequency;
            const double fy = sy / frequency;

            uint32_t pixels[sy][sx];



            for (int y = 0; y < sy; ++y){
                for (int x = 0; x < sx; ++x){
                    double ret = perlin.octaveNoise0_1(x / fx, y / fy, octaves) * 255.0;
                    pixels[y][x] = RenderHelp::FormatRGBA(ret,ret,ret,255);
                }
            }
            TextureLoadMethod filter(TEXTURE_TRILINEAR);

            t =  Painter::MakeTexture(PointInt(sx,sy), GL_RGBA ,(unsigned char*)pixels, filter);

            texture = chain_ptr<BearTexture>::make(t);

            sp = Sprite(texture, std::string("ewe"));

        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                //requestDelete = true;
            }
            trasher = g_input.GetMouse().y /(float)SCREEN_SIZE_H;

        };
        void Render(){
            RenderHelp::DrawSquareColor(Rect(0,0,1000,1000),90,90,90,255);
            shdr.Bind();
            shdr.SetUniform<float>("trash", trasher);

            ShaderSetter<BearTexture>::SetUniform(Shader::GetCurrentShaderId(),"image2",(*sp2.GetTexture().get()),1);


            sp.Render(PointInt(32,32));
            shdr.Unbind();
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float trasher;
        Shader shdr;
        float duration;
        BearTexture *t;
        TexturePtr texture;
        Sprite sp;
        Sprite sp2;
};

#endif // DISABLE_UNIT_TESTS

