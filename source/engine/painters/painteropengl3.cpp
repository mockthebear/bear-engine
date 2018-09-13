#include "../../settings/definitions.hpp"

#ifdef RENDER_OPENGL3
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
uint32_t Painter::Buffers[4] = {0, 0, 0, 0};



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
        case SUPPORT_POINTSPRITE:
            return true;
        default:
            return false;
    }
}


void RenderData::Bind(){
    glBindVertexArray(VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
}

RenderData::~RenderData(){
    if (VertexArray != 0){
        glDeleteBuffers(1, &VertexBuffer);
        glDeleteBuffers(1, &ElementBuffer);
        glDeleteVertexArrays(1, &VertexArray);
        VertexArray = 0;
    }
};

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
        0.0f                , 0.0f                  , texLeft   , texTop,
        0.0f                , m_scale.y * size.y    , texLeft   , texBottom,
        m_scale.x * size.x  , m_scale.y * size.y    , texRight  , texBottom,
        m_scale.x * size.x  , 0.0f                  , texRight  , texTop,

    };

    float auxa = Geometry::toRad(m_angle);

    float s = sin(auxa);
    float c = cos(auxa);

    for (int i=0;i<4;i++){
        float px = vertices[i * 4  + 0] -= center.x;
        float py = vertices[i * 4  + 1] -= center.y;
        vertices[i * 4  + 0] = (px * c - py * s) + center.x + position.x;
        vertices[i * 4  + 1] = (px * s + py * c) + center.y + position.y;
    }


    if (VertexArray == 0){
        static unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        glGenVertexArrays(1, &VertexArray);
        glGenBuffers(1, &VertexBuffer);
        glGenBuffers(1, &ElementBuffer);

        glBindVertexArray(VertexArray);

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
        glBindVertexArray(0);
    }else{
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}


void Painter::DrawVertex(VertexArrayObjectPtr vertexData,BasicRenderDataPtr t_data,int drawMode,bool isBound){

    bool noShader = Shader::GetCurrentShaderId() == 0;
    DisplayGlError("aa");
    if (noShader){
        polygonShader.Bind();
    }

    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Projection);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);

    if (!isBound){
        vertexData->Bind();
    }

    glDrawElements(drawMode, vertexData->GetIndexCount(), GL_UNSIGNED_INT, 0);

    if (noShader){
        polygonShader.Unbind();
    }



}

bool Painter::RenderPointTexture(BearTexture *t_texture, RenderDataPtr t_data){
    if (!t_texture || t_texture->id == 0){
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
    glActiveTexture(GL_TEXTURE0);
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

    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        textureShader.Bind();
    }

    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Projection);
    ShaderSetter<BearColor>::SetUniform(Shader::GetCurrentShaderId(),"iColor",t_data->color);
    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    t_data->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if (noShader){
        textureShader.Unbind();
    }

    return true;
}


void Painter::SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels ){
    if (!pixels){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    filter.ApplyFilter();

    glTexImage2D(GL_TEXTURE_2D, 0, mode, pow_w, pow_h, 0, mode, GL_UNSIGNED_BYTE, nullptr);
    if (pixels != nullptr){
        SetTexturePixels(texId, size, mode, pixels);

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


    DisplayGlError("1");

	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable( GL_DEPTH_TEST );
    //glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &sm->m_vertexArrayID);
    glBindVertexArray(sm->m_vertexArrayID);


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

        pointTextureShader.Compile(GL_VERTEX_SHADER, "data/shaders/ps_texture_vertex.glfs");
        pointTextureShader.Compile(GL_GEOMETRY_SHADER, "data/shaders/ps_texture_geometry.glfs");
        pointTextureShader.Compile(GL_FRAGMENT_SHADER, "data/shaders/ps_texture_frag.glfs");
        pointTextureShader.Link();

        Painter::m_vao.DisableElements();

        Painter::m_vao.SetupVertexes();

        Painter::m_vao.Bind();

        int elements = 11;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), 0); //Position
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(2 * sizeof(float))); //Vertex clip
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(6 * sizeof(float))); //image size
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(8 * sizeof(float))); //scale
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, elements * sizeof(float), (void*)(10 * sizeof(float))); //rotation
        Painter::m_vao.UnBind();

        bear::out << "Making buffers\n";
        glGenBuffers(4, Painter::Buffers);
        DisplayGlError("SetupEnvoriment");

    }
}

#endif // RENDER_OPENGL3
