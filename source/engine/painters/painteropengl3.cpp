#include "../../settings/definitions.hpp"

//#ifdef RENDER_OPENGL3
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../renderhelp.hpp"
#include "../../input/inputmanager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../framework/debughelper.hpp"

bool Painter::m_shaderBuilt = false;
Shader Painter::textureShader;
Shader Painter::polygonShader;

void Painter::DrawSquare(Rect box,int r,int g,int b,int a){
    static GLuint VAO;
    static GLuint VBO;
    static bool made = false;


    glm::vec2 size(box.w,box.h);
    glm::mat4 model(1.0f);
    glm::mat4 projection;


    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    if (!made){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        made = true;

    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }


    Point scale(1,1);

    model = glm::translate(model, glm::vec3(box.x, box.y, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size.x * scale.x,size.y * scale.y, 1.0f));

    Point scr = ScreenManager::GetInstance().GetGameSize();
    projection = glm::ortho(0.0f, (float)scr.x,  (float)scr.y, 0.0f, -1.0f, 1.0f);



    unsigned int transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"spriteColor",BearColor(r/255.0f,g/255.0f,b/255.0f,a/255.0f));

    glBindVertexArray(VAO);
    //if (outline){
    //    glDrawArrays(GL_LINE_LOOP, 0, 8);
    //}else{
        glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
    //}
    glBindVertexArray(0);

    if (noShader){
        polygonShader.Unbind();
    }
}

bool Painter::RenderTexture(BearTexture *t_texture, RenderData &t_data){
    if (!t_texture){
        return false;
    }

    static GLuint VAO;
    static GLuint VBO;
    static bool made = false;

    if (!t_texture || t_texture->id == 0){
        return false;
    }
    static glm::vec2 size;
    static glm::mat4 projection;
    static glm::mat4 model;

    size = glm::vec2 (t_data.clip.w,t_data.clip.h);
    model = glm::mat4(1.0f);


    float texLeft = t_data.forwardClip.x;
    float texRight =  t_data.forwardClip.y;
    float texTop = t_data.forwardClip.w;
    float texBottom = t_data.forwardClip.h;


    if ((t_data.flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((t_data.flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }

    GLfloat vertices[] = {


        // Pos      // Tex
        0.0f, 0.0f, texLeft, texTop,
        0.0f, 1.0f, texLeft, texBottom,
        1.0f, 1.0f, texRight, texBottom,
        1.0f, 0.0f, texRight, texTop,

    };

    if (!made){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        made = true;



    }


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        textureShader.Bind();
    }

    if( InputManager::GetInstance().IsKeyDown(SDLK_a)  ) {

    model = glm::translate(model, glm::vec3(t_data.position.x, t_data.position.y, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, t_data.angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size.x * t_data.scale.x,size.y * t_data.scale.y, 1.0f));
    Point scr = ScreenManager::GetInstance().GetGameSize();
    projection = glm::ortho(0.0f, (float)scr.x,  (float)scr.y, 0.0f, -1.0f, 1.0f);




    unsigned int transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    BearColor recolor(t_data.color[0],t_data.color[1],t_data.color[2],t_data.color[3]);
    ShaderSetter<BearColor>::SetUniform(textureShader.GetCurrentShaderId(),"spriteColor",recolor);
    ShaderSetter<int>::SetUniform(textureShader.GetCurrentShaderId(),"image",0);






    /*unsigned int ow = glGetUniformLocation(Shader::GetCurrentShaderId(), "OwO");
    std::cout << ow << " : "<<Shader::GetCurrentShaderId()<<"\n";
    glUniform1f(ow,0.3f);*/



    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    }

    if (noShader){
        textureShader.Unbind();
    }


    return true;
}

BearTexture* Painter::MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter){
    if (size.x == 0 || size.y == 0){
        return nullptr;
    }
    GLuint texId = 0;
    glGenTextures(1, &texId);
    if (texId == 0){
        return nullptr;
    }

    unsigned int pow_w =  size.x;//powerOfTwo(size.x);
    unsigned int pow_h =  size.y;//powerOfTwo(size.y); //No need to this on opengl. only on opengles

    glBindTexture(GL_TEXTURE_2D, texId);
    filter.ApplyFilter();
    glTexImage2D(GL_TEXTURE_2D, 0, mode, pow_w, pow_h, 0, mode, GL_UNSIGNED_BYTE, nullptr);
    if (pixels != nullptr){
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    BearTexture *ret = new BearTexture(texId,size.x,size.y,pow_w,pow_h,mode);
    ret->textureMode = filter;
    return ret;
}


bool Painter::SetupEnvoriment(ScreenManager *sm){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(0);


    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK ){
        bear::out << "Error initializing GLEW!\n";
        return false;
    }


    DebugHelper::DisplayGlError("1");

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &sm->m_vertexArrayID);
    glBindVertexArray(sm->m_vertexArrayID);

    SetupShaders();

    return true;
}

int Painter::GetMaxTextureSize(){

    return 1;
}

void Painter::ResetViewPort(PointInt originalSize, PointInt newSize){

}

void Painter::SetupShaders(){
    if (!m_shaderBuilt){
        m_shaderBuilt = true;
        polygonShader.Compile(GL_VERTEX_SHADER,"quadvertex.glvs");
        polygonShader.Compile(GL_FRAGMENT_SHADER,"quadfrag.glfs");
        polygonShader.Link();
        textureShader.Compile(GL_VERTEX_SHADER,"sprvertex.glvs");
        textureShader.Compile(GL_FRAGMENT_SHADER,"sprfrag.glfs");
        textureShader.Link();
    }
}

//#endif // RENDER_OPENGL