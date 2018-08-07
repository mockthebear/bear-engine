#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/shadermanager.hpp"
#include "../performance/linegrap.hpp"
#pragma once

class FastTiles{
    public:
        FastTiles(){

        };

};

class Test_FastTiles: public State{
    public:
        Test_FastTiles(){
            requestQuit = requestDelete = false;
            bear::out << "Test\n";
            duration = 10.0f;
            offst = 0;
            gCount = 0;
            checkFrame = false;
        };
        ~Test_FastTiles(){

        };
        void Begin(){
            lg = LineGraph(Point(500,200),200);



            m_renderTimer = LineGraph(Point(500,200),200);
            ScreenManager::GetInstance().SetScreenName("Test fast tile");
            m_shader.Compile(GL_VERTEX_SHADER, "data/shaders/ft_vertex.glfs");
            m_shader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultTextureFragmentShader);
            m_shader.Link();
            bear = Sprite("data/bear.png", TextureLoadMethod(TEXTURE_TRILINEAR));
            tilesCount = Text("ui/UnB-Office_Regular.ttf",22,TEXT_SOLID,"a",{255,33,180});
            AddBear();



        };

        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                for (int i=0;i<50;i++){
                    AddBear();
                }
                duration = 5.0f;
                m_renderTimer.AddData(gCount/10.0f);
                lg.AddData(ScreenManager::GetInstance().GetFps());
                gCount = 0;
                std::string txt = utils::format("FPS: %f  : Tiles: %d",ScreenManager::GetInstance().GetFps(), indexes.size()/6 );
                tilesCount.SetText(txt);
                checkFrame = true;
            }else{
                checkFrame = false;
            }

        };

        void AddBear(){
            float texLeft = 0;
            float texRight =  1;
            float texTop = 0;
            float texBottom = 1;

            float rX = rand()%800 - 16;
            float rY = rand()%600 - 16;


            float vertices[] = {
                // Pos      // Tex
                0.0f  , 0.0f    , texLeft   , texTop,       rX,     rY,
                0.0f  , 1.0f    , texLeft   , texBottom,    rX,     rY,
                1.0f  , 1.0f    , texRight  , texBottom,    rX,     rY,
                1.0f  , 0.0f    , texRight  , texTop,       rX,     rY,

            };




            unsigned int indices[] = {
                offst, offst+1, offst+3,
                offst+1, offst+2, offst+3,
            };
            offst += 4;


            vertexData.insert(vertexData.begin(), &vertices[0], &vertices[24]);
            indexes.insert(indexes.begin(), &indices[0], &indices[6]);
        }

        void Render(){
            sw.Reset();
            //bear.Render(Point(8,8),0);

            VertexBuffer = Painter::GetSharedBuffer(0);
            ElementBuffer = Painter::GetSharedBuffer(1);
            DebugHelper::DisplayGlError("on get");



            glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,  vertexData.size() * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
            DebugHelper::DisplayGlError("on set array");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indexes.size() * sizeof(uint32_t), &indexes[0] , GL_DYNAMIC_DRAW);
            DebugHelper::DisplayGlError("on set buffer");

            GLint posAttrib = 0;
            GLint clipAttrib = 1;
            GLint transLAttrib = 2;

            m_shader.Bind();



            #ifdef NEED_SHADER_LOCATION


            uint32_t shaderId = m_shader.GetId();


            /*if ((shaderId = Shader::GetCurrentShaderId()) == 0){
                shaderId = Painter::textureShader.GetId();
            }*/

            posAttrib = glGetAttribLocation(shaderId, "vPos");
            clipAttrib = glGetAttribLocation(shaderId, "clip");
            transLAttrib = glGetAttribLocation(shaderId, "translate");
            DebugHelper::DisplayGlError("on get attrs");
            #endif // NEED_SHADER_LOCATION


            glEnableVertexAttribArray(posAttrib);
            glVertexAttribPointer(posAttrib,    2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)0);
            DebugHelper::DisplayGlError("on set attr 1");
            glEnableVertexAttribArray(clipAttrib);
            glVertexAttribPointer(clipAttrib,   2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
            DebugHelper::DisplayGlError("on set attr 2");
            glEnableVertexAttribArray(transLAttrib);
            glVertexAttribPointer(transLAttrib, 2,    GL_FLOAT, GL_FALSE,   6 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)) );
            DebugHelper::DisplayGlError("on set attr 3");


            RenderDataPtr data = bear.GetRenderData();



            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bear.GetTexture().get()->id);



            glm::mat4 mm = data->GetModel();
            ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Painter::Projection);
            ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",mm);
            ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",data->color);


            glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, nullptr);
            DebugHelper::DisplayGlError("on rebder");


            m_shader.Unbind();

            gCount += sw.Get();

            if (g_input.IsKeyDown(SDLK_1))
                lg.Render(Point(32,32));
            if (g_input.IsKeyDown(SDLK_2))
                m_renderTimer.Render(Point(32,264));
            if (g_input.IsKeyDown(SDLK_3))
                tilesCount.Render(Point(64,64));

        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Stopwatch sw;
        bool checkFrame;
        LineGraph lg;
        LineGraph m_renderTimer;
        std::vector<float> vertexData;
        std::vector<uint32_t> indexes;
        float duration;
        float gCount;
        Text tilesCount;
        uint32_t VertexBuffer;
        uint32_t ElementBuffer;
        uint32_t offst;
        Shader m_shader;
        Sprite bear;
};

