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
        //model = glm::rotate(glm::mat4(1.0f), m_angle, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(glm::mat4(1.0f), glm::vec3(m_clip.w * m_scale.x,m_clip.h * m_scale.y, 1.0f));
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
        0.0f, 0.0f, texLeft, texTop,
        0.0f, 1.0f, texLeft, texBottom,
        1.0f, 1.0f, texRight, texBottom,
        1.0f, 0.0f, texRight, texTop,

    };

    glBindBuffer(GL_ARRAY_BUFFER, Painter::TextureVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



bool Painter::m_shaderBuilt = false;
Shader Painter::textureShader;
Shader Painter::polygonShader;

GLuint Painter::SharedVertexArray;
GLuint Painter::SharedVertexBuffer;

GLuint Painter::TextureVertexArray;
GLuint Painter::TextureVertexBuffer;



void Painter::SetupPolygonVAOs(){

    //glGenVertexArrays(1, &SharedVertexArray);

    glGenBuffers(1, &SharedVertexBuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, SharedVertexBuffer);

   // glBindVertexArray(SharedVertexArray);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);



    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,

    };
    glGenVertexArrays(1, &TextureVertexArray);
    glGenBuffers(1, &TextureVertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, TextureVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glBindVertexArray(TextureVertexArray);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




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


bool Painter::RenderTexture(BearTexture *t_texture, RenderData &t_data){
    if (!t_texture || t_texture->id == 0){
        return false;
    }

    bool noShader = Shader::GetCurrentShaderId() == 0;
    if (noShader){
        textureShader.Bind();
    }

    t_data.UpdateVertex();
    t_data.UpdateModel();
    //Translate and scale
    glm::mat4 model = glm::translate(t_data.model, glm::vec3(t_data.position.x, t_data.position.y, 0.0f));


    //Get current projection
    glm::mat4& projection = ScreenManager::GetInstance().GetProjection();

    //Setup shader
    unsigned int transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

    transformLoc = glGetUniformLocation(textureShader.GetCurrentShaderId(), "projection");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    BearColor recolor(t_data.color[0],t_data.color[1],t_data.color[2],t_data.color[3]);
    ShaderSetter<BearColor>::SetUniform(textureShader.GetCurrentShaderId(),"spriteColor",recolor);
    //ShaderSetter<int>::SetUniform(textureShader.GetCurrentShaderId(),"image",0);

    //Now render the sprite itself

    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    glBindVertexArray(Painter::TextureVertexArray);


    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);


    if (noShader){
        textureShader.Unbind();
    }


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

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);

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
