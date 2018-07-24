#include "../../settings/definitions.hpp"

#ifdef RENDER_OPENGL3
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../renderhelp.hpp"
#include "../../input/inputmanager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../framework/debughelper.hpp"


void RenderData::UpdateModel(){
    if (m_modelUpdateNeeded){

        m_modelUpdateNeeded = false;
    }
}

void RenderData::UpdateVertex(){

    float texLeft = m_forwardClip.x;
    float texRight =  m_forwardClip.y;
    float texTop = m_forwardClip.w;
    float texBottom = m_forwardClip.h;


    if ((flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }

    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f  , 0.0f    , texLeft   , texTop,
        0.0f  , 1.0f    , texLeft   , texBottom,
        1.0f  , 1.0f    , texRight  , texBottom,
        1.0f  , 0.0f    , texRight  , texTop,

    };


    if (TextureVertexArray == 0){
        static unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        glGenVertexArrays(1, &TextureVertexArray);
        glGenBuffers(1, &TextureVertexBuffer);
        glGenBuffers(1, &TextureElementBuffer);

        glBindVertexArray(TextureVertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, TextureVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TextureElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }else{
        glBindBuffer(GL_ARRAY_BUFFER, TextureVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}



bool Painter::m_shaderBuilt = false;
Shader Painter::textureShader;
Shader Painter::polygonShader;

GLuint Painter::SharedVertexArray;
GLuint Painter::SharedVertexBuffer;



void Painter::SetupPolygonVAOs(){
    glGenBuffers(1, &SharedVertexBuffer);
}

void Painter::DrawLine(Point p1,Point p2,BearColor color,float thicc){
    /*glm::mat4 model(1.0f);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }

    Rect box(p1.x,p1.y,p2.x-p1.x, p2.y-p1.y);


    glm::mat4& projection = ScreenManager::GetInstance().GetProjection();




    float angle =  p1.GetDirection(p2);


    model = glm::translate(model, glm::vec3(box.x, box.y, 0.0f));
    if (angle != 0){

        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

    }
    model = glm::scale(model, glm::vec3(box.w,box.h, 1.0f));



    unsigned int transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"spriteColor",color);

    glBindVertexArray(VAO_2);


    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);

    if (noShader){
        polygonShader.Unbind();
    }*/

}

void Painter::DrawSquare(Rect box,BearColor color,bool outline,float angle){
    /*
    glm::mat4 model(1.0f);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }

    model = glm::scale(model, glm::vec3(box.w,box.h, 1.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(box.x, box.y, 0.0f));


    glm::mat4& projection = ScreenManager::GetInstance().GetProjection();



    unsigned int transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(Shader::GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"spriteColor",color);

    glBindVertexArray(VAO_4);
    if (outline){
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }else{
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);

    if (noShader){
        polygonShader.Unbind();
    }*/
}


bool Painter::RenderTexture(BearTexture *t_texture, RenderDataPtr t_data){
    if (!t_texture || t_texture->id == 0){
        return false;
    }

    //Benchmark
    //
    // Bind: 0.00165~
    // Texture bind: 0
    // Bind texture: 0.0003
    // Pssar tudo pro shader: 0.009~
    // Só nao desenhando: 0.010~
    // Chamando tudo com draw elements 0.020
    // Com operaçoes de model: 0.30



    glm::mat4 model(1.0f);




    model[3][0] += t_data->position.x;
    model[3][1] += t_data->position.y;
    model[3][2] += 0.0f;



    if (t_data->m_angle != 0){


        model[3][0] += 0.5f * t_data->m_clip.w;
        model[3][1] += 0.5f * t_data->m_clip.h;

        float theta = glm::radians(t_data->m_angle);

        model[0][0] = cos(theta);
        model[1][0] = -sin(theta);
        model[0][1] = sin(theta);
        model[1][1] = cos(theta);

        float a = -0.5f * t_data->m_clip.w;
        float b = -0.5f * t_data->m_clip.h;


        model[3][1] += (b * model[0][0] + (a * model[0][1]));
        model[3][0] += (b * model[1][0] + (a * model[1][1]));

    }

    model[0][0] = model[0][0] * (t_data->m_scale.x * t_data->m_clip.w);
    model[1][0] = model[1][0] * (t_data->m_scale.y * t_data->m_clip.h);
    model[0][1] = model[0][1] * (t_data->m_scale.x * t_data->m_clip.w);
    model[1][1] = model[1][1] * (t_data->m_scale.y * t_data->m_clip.h);


    //Setup shader

    textureShader.Bind();

    unsigned int transformLoc;
    glm::mat4& projection = ScreenManager::GetInstance().GetProjection();
    transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    BearColor recolor(t_data->color[0],t_data->color[1],t_data->color[2],t_data->color[3]);
    ShaderSetter<BearColor>::SetUniform(textureShader.GetCurrentShaderId(),"spriteColor",recolor);


    ShaderSetter<int>::SetUniform(textureShader.GetCurrentShaderId(),"image",0);


    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    glBindVertexArray(t_data->TextureVertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    return true;
}

static void DrawVertex(Vertex &v,BearColor c,int drawMode){

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

	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable( GL_DEPTH_TEST );
    //glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &sm->m_vertexArrayID);
    glBindVertexArray(sm->m_vertexArrayID);


    SetupShaders();
    SetupPolygonVAOs();

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

#endif // RENDER_OPENGL3
