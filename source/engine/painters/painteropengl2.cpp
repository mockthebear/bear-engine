#ifdef RENDER_OPENGL2
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../../framework/debughelper.hpp"

glm::mat4 Painter::Projection;
bool Painter::m_shaderBuilt = false;
Shader Painter::textureShader;
Shader Painter::polygonShader;
Shader Painter::pointTextureShader;
uint32_t Painter::Buffers[4] = {0, 0, 0, 0};

uint32_t Painter::GetSharedBuffer(int id){
    return Buffers[id];
}

void RenderData::Bind(){
    DebugHelper::DisplayGlError("Pre bind");
    #ifdef SUPPORT_SINGLE_BUFFER
    VertexBuffer = Painter::GetSharedBuffer(0);
    ElementBuffer = Painter::GetSharedBuffer(1);
    #endif
    DebugHelper::DisplayGlError("Binding");
    UpdateVertex();
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
}

RenderData::~RenderData(){
    #ifndef SUPPORT_SINGLE_BUFFER
    if (VertexBuffer != 0){
        glDeleteBuffers(1, &VertexBuffer);
        glDeleteBuffers(1, &ElementBuffer);
    }
    #endif // SUPPORT_SINGLE_BUFFER
};

void RenderData::SetVertexAtribLocations(){
    GLint posAttrib = 0;
    GLint clipAttrib = 1;
    #ifdef NEED_SHADER_LOCATION

    uint32_t shaderId = 0;
    if ((shaderId = Shader::GetCurrentShaderId()) == 0){
        shaderId = Painter::textureShader.GetId();
    }
    posAttrib = glGetAttribLocation(shaderId, "vPos");
    clipAttrib = glGetAttribLocation(shaderId, "clip");
    #endif // NEED_SHADER_LOCATION


    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(clipAttrib);
    glVertexAttribPointer(clipAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );

    DebugHelper::DisplayGlError("SetVertexAtribLocations");
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

    #ifdef SUPPORT_SINGLE_BUFFER
    VertexBuffer = Painter::GetSharedBuffer(0);
    ElementBuffer = Painter::GetSharedBuffer(1);
    static unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
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


    #else
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
        SetVertexAtribLocations();

    }else{
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        SetVertexAtribLocations();
    }
    #endif // SUPPORT_SINGLE_BUFFER
    DebugHelper::DisplayGlError("UpdateVertex");
}


void BasicRenderData::UpdateModel(){

    m_model = glm::mat4(1.0f);
    if (m_angle != 0){


        m_model[3][0] += 0.5f * size.x;
        m_model[3][1] += 0.5f * size.y;

        float theta = glm::radians(m_angle);

        m_model[0][0] = cos(theta);
        m_model[1][0] = -sin(theta);
        m_model[0][1] = sin(theta);
        m_model[1][1] = cos(theta);

        float a = -0.5f * size.x;
        float b = -0.5f * size.y;


        m_model[3][1] += (b * m_model[0][0] + (a * m_model[0][1]));
        m_model[3][0] += (b * m_model[1][0] + (a * m_model[1][1]));

    }

    m_model[0][0] = m_model[0][0] * (m_scale.x * size.x);
    m_model[1][0] = m_model[1][0] * (m_scale.y * size.y);
    m_model[0][1] = m_model[0][1] * (m_scale.x * size.x);
    m_model[1][1] = m_model[1][1] * (m_scale.y * size.y);

}


glm::mat4 Painter::CalculateModel(BasicRenderDataPtr t_data){
    glm::mat4 model(1.0f);

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


    model[3][0] += t_data->position.x;
    model[3][1] += t_data->position.y;
    model[3][2] = 0.0f;

    return model;
}



void Painter::SetViewport(Point screenNow,Point offset){
    glViewport(offset.x, offset.y, screenNow.x, screenNow.y);
}

void Painter::SetProjection(Rect rproj){
    Painter::Projection = glm::ortho(rproj.x, rproj.y, rproj.w, rproj.h, -1.0f, 1.0f);
}

void Painter::SetProjection(Point size,int flipScreen){
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
        case SUPPORT_ORTOHOVERTEX:
            return false;
        default:
            return false;
    }
}

void Painter::DrawVertex(VertexArrayObjectPtr vertexData,BasicRenderDataPtr t_data,int drawMode, bool ignoreModel){
    glm::mat4 model;
    if (!ignoreModel){
       model = CalculateModel(t_data);
    }else{
        model = glm::mat4(1.0f);
    }

    bool noShader = Shader::GetCurrentShaderId() == 0;

    DebugHelper::DisplayGlError("on shader");

    if (noShader){
        polygonShader.Bind();
    }

    DebugHelper::DisplayGlError("set uniforms");
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Projection);
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",model);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);

    vertexData->Bind();


    glDrawElements(drawMode, vertexData->GetIndexCount(), GL_UNSIGNED_INT, 0);
    DebugHelper::DisplayGlError("DrawVertex");
    if (noShader){
        polygonShader.Unbind();
    }
    DebugHelper::DisplayGlError("Unbind");

}


bool Painter::RenderPointTexture(BearTexture *t_texture, RenderDataPtr t_data){
    return false;

}

bool Painter::DrawSprites(int id){
    return false;
}

bool Painter::RenderTexture(BearTexture *t_texture, RenderDataPtr t_data){



    glm::mat4 model = t_data->GetModel();

    model[3][0] += t_data->position.x;
    model[3][1] += t_data->position.y;
    model[3][2] = 0.0f;

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        textureShader.Bind();
    }
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Projection);
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"model",model);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    t_data->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture( GL_TEXTURE_2D, 0 );

    if (noShader){
        textureShader.Unbind();
    }
    DebugHelper::DisplayGlError("RenderTexture");
    return true;
}

void Painter::SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels ){
    if (!pixels){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    DebugHelper::DisplayGlError("SetTexturePixels");
}

BearTexture* Painter::MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter){
    if (size.x == 0 || size.y == 0){
        return nullptr;
    }
    GLuint texId = 0;
    glGenTextures(1, &texId);
    DebugHelper::DisplayGlError("glGenTextures");
    if (texId == 0){
        return nullptr;
    }

    unsigned int pow_w = powerOfTwo(size.x);
    unsigned int pow_h = powerOfTwo(size.y); //No need to this on opengl. only on opengles

    glBindTexture(GL_TEXTURE_2D, texId);
    filter.ApplyFilter();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, pow_w, pow_h, 0, mode, GL_UNSIGNED_BYTE, nullptr);
    if (pixels != nullptr){

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    BearTexture *ret = new BearTexture(texId,size.x,size.y,pow_w,pow_h,mode);
    ret->textureMode = filter;
    DebugHelper::DisplayGlError("MakeTexture");
    return ret;
}


bool Painter::SetupEnvoriment(ScreenManager *sm){
    bear::out << "Initializing\n";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(0);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        bear::out << "Error initializing GLEW!\n";
        return false;
    }

    DebugHelper::DisplayGlError("1");

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bear::out << "Loading shaders\n";
    SetupShaders();

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	DebugHelper::DisplayGlError("Error on starting");

    bear::out << "Making buffers\n";
	glGenBuffers(4, Painter::Buffers);
	DebugHelper::DisplayGlError("SetupEnvoriment");
    return true;
}


void Painter::SetupShaders(){
    if (!m_shaderBuilt){
        m_shaderBuilt = true;
        bear::out << "[QUAD shader]Building vertex shader\n";
        polygonShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultQuadVertexShader);
        bear::out << "[QUAD shader]Building frag shader\n";
        polygonShader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultQuadFragmentShader);
        bear::out << "[QUAD shader]linking\n";
        polygonShader.Link();
        bear::out << "[TEXTURE shader]Building vertex shader\n";
        textureShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
        bear::out << "[TEXTURE shader]Building frag shader\n";
        textureShader.CompileFromString(GL_FRAGMENT_SHADER, Shader::DefaultTextureFragmentShader);
        bear::out << "[TEXTURE shader]linking\n";
        textureShader.Link();
    }
}

int Painter::GetMaxTextureSize(){
    int maxSize;
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxSize);
    return maxSize;
}



#endif // RENDER_OPENGL

