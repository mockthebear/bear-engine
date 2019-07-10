
#include "../../settings/definitions.hpp"

#ifdef RENDER_OPENGLES2
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../renderhelp.hpp"
#include "../../input/inputmanager.hpp"

#include "../../framework/debughelper.hpp"


bool Painter::m_shaderBuilt = false;
Shader Painter::textureShader;
Shader Painter::polygonShader;
Shader Painter::pointTextureShader;
glm::mat4 Painter::Projection;
VertexArrayObject Painter::m_vao;






void Painter::SetViewport(Point size,int flipScreen,Point screenNow,Point offset){
    glViewport(offset.x, offset.y, screenNow.x, screenNow.y);
    Point xAxis;
    Point yAxis;
    if (flipScreen&SDL_FLIP_HORIZONTAL){
        xAxis.x = size.x;
        xAxis.y = 0.0f;
    }else{
        xAxis.y = size.x;
        xAxis.x = 0.0f;
    }
    if (flipScreen&SDL_FLIP_VERTICAL){
        yAxis.y = size.x;
        yAxis.x = 0.0f;
    }else{
        yAxis.x = size.y;
        yAxis.y = 0.0f;
    }
    Painter::Projection = glm::ortho(xAxis.x, xAxis.y, yAxis.x, yAxis.y, -1.0f, 1.0f);
}

bool Painter::CanSupport(PainterSupport sup){
    switch(sup){
        case SUPPORT_SHADERS:
            return true;
        case SUPPORT_VERTEXBUFFER:
            return true;
        case SUPPORT_POINTSPRITE:
            return true;
        default:
            return false;
    }
}

RenderData::~RenderData(){
    if (VertexBuffer != 0){
        glDeleteBuffers(1, &VertexBuffer);
        glDeleteBuffers(1, &ElementBuffer);
    }
};

void RenderData::Bind(){
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
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


    if (VertexBuffer == 0){
        static unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenBuffers(1, &VertexBuffer);
        glGenBuffers(1, &ElementBuffer);



        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }else{
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}





glm::mat4 Painter::CalculateModel(BasicRenderDataPtr t_data){
    glm::mat4 model(1.0f);


    model[3][0] += t_data->position.x;
    model[3][1] += t_data->position.y;
    model[3][2] += 0.0f;



    if (t_data->m_angle != 0){


        model[3][0] += 0.5f * t_data->size.x;
        model[3][1] += 0.5f * t_data->size.y;

        float theta = glm::radians(t_data->m_angle);

        model[0][0] = cos(theta);
        model[1][0] = -sin(theta);
        model[0][1] = sin(theta);
        model[1][1] = cos(theta);

        float a = -0.5f * t_data->size.x;
        float b = -0.5f * t_data->size.y;


        model[3][1] += (b * model[0][0] + (a * model[0][1]));
        model[3][0] += (b * model[1][0] + (a * model[1][1]));

    }

    model[0][0] = model[0][0] * (t_data->m_scale.x * t_data->size.x);
    model[1][0] = model[1][0] * (t_data->m_scale.y * t_data->size.y);
    model[0][1] = model[0][1] * (t_data->m_scale.x * t_data->size.x);
    model[1][1] = model[1][1] * (t_data->m_scale.y * t_data->size.y);
    return model;
}

void Painter::DrawVertex(VertexArrayObjectPtr vertexData,BasicRenderDataPtr t_data,int drawMode){

    glm::mat4 model = CalculateModel(t_data);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        polygonShader.Bind();
    }

    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"g_projection",Projection);
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",model);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);



    vertexData->Bind();
    glDrawElements(drawMode, vertexData->GetIndexCount(), GL_UNSIGNED_INT, 0);

    if (noShader){
        polygonShader.Unbind();
    }



}

bool Painter::RenderPointTexture(BearTexture *t_texture, RenderDataPtr t_data){
    if (!t_texture){
        return false;
    }
    float points[] ={
        t_data->position.x, t_data->position.y,
        t_data->m_forwardClip.x, t_data->m_forwardClip.y, t_data->m_forwardClip.w, t_data->m_forwardClip.h,
        t_data->m_clip.w, t_data->m_clip.h,
        t_data->m_scale.x, t_data->m_scale.y,
        t_data->m_angle,

    };
    m_vao.vertexes.AddVertexes( sizeof(points) / sizeof(float) ,points);

    return true;
}


bool Painter::DrawSprites(int id){

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, id );


    m_vao.SetupVertexes();


    pointTextureShader.Bind();
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);
    Point sz = ScreenManager::GetInstance().GetGameSize();
    sz.x = (1.0f / sz.x) * 2.0;  //2.0 because its from -1 to 1.
    sz.y = (1.0f / sz.y) * 2.0;  //2.0 because its from -1 to 1.
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"orthoVec",sz);

    m_vao.Bind();

    glDrawArrays(GL_POINTS, 0, m_vao.GetVertexesCount()/11);
    pointTextureShader.Unbind();
    m_vao.UnBind();


    m_vao.clear();

    return true;
}


bool Painter::RenderTexture(BearTexture *t_texture, RenderDataPtr t_data){
    if (!t_texture || t_texture->id == 0){
        return false;
    }
    glEnable(GL_TEXTURE_2D);

    glm::mat4 model = CalculateModel(t_data);

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        textureShader.Bind();
    }

    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"g_projection",Projection);
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",model);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);



    uint32_t texId = t_texture->id;
    if (texId == 0){
        texId = Painter::UnloadedTextureId;
    }
    glBindTexture( GL_TEXTURE_2D, texId );

    t_data->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

    unsigned int pow_w =  powerOfTwo(size.x);
    unsigned int pow_h =  powerOfTwo(size.y); //No need to this on opengl. only on opengles

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

    //SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
    SDL_SetHint(SDL_HINT_VIDEO_WIN_D3DCOMPILER, "none");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   // glewExperimental = GL_TRUE;
   /* GLenum glewError = glewInit();
    if( glewError != GLEW_OK ){
        bear::out << "Error initializing GLEW!\n";
        return false;
    }*/


    DisplayGlError("1");

	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable( GL_DEPTH_TEST );
    //glEnable(GL_CULL_FACE);



    SetupShaders();
    return true;
}

int Painter::GetMaxTextureSize(){
    int maxSize;
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxSize);
    return maxSize;
}


void Painter::SetupShaders(){
    if (!m_shaderBuilt){
        m_shaderBuilt = true;

        polygonShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultQuadVertexShader);
        polygonShader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultQuadFragmentShader);
        polygonShader.Link();

        textureShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
        textureShader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultTextureFragmentShader);
        textureShader.Link();

    }
}

#endif // RENDER_OPENGL3

