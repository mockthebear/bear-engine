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

glm::mat4 Painter::Projection;
VertexArrayObject Painter::m_vao;
uint32_t Painter::Buffers[4] = {0, 0, 0, 0};
uint32_t Painter::UnloadedTextureId = 0;

uint32_t Painter::GetSharedBuffer(int id){
    return Buffers[id];
}


bool Painter::RenderTexture(BearTexture *t_texture, VertexArrayObjectPtr vertexData){

    if (!t_texture){
        t_texture = Painter::UnloadedTexture;
        if (!t_texture){
            return false;
        }
    }
    DisplayGlError("Pre rendering");
    bool noShader = Shader::GetCurrentShaderId() == 0;

    if (noShader){
        textureShader.Bind();
    }

    DisplayGlError("Bound");
    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"g_projection",Projection);

    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image", 0);

    DisplayGlError("Set uniforms");

    glActiveTexture(GL_TEXTURE0);

    DisplayGlError("Activated");

    uint32_t texId = t_texture->id;
    if (texId == 0){
        texId = Painter::UnloadedTextureId;
    }

    glBindTexture( GL_TEXTURE_2D, texId );


    vertexData->Bind();
    vertexData->SetupVertexes();
    Vertex::SetAttributes();

    glDrawElements(GL_TRIANGLES, vertexData->GetDrawElementsCount(), GL_UNSIGNED_INT, 0);

    glBindTexture( GL_TEXTURE_2D, 0 );
    vertexData->UnBind();

    if (noShader){
        textureShader.Unbind();
    }
    DisplayGlError("RenderTexture");
    return true;
}



void Painter::DrawVertex(VertexArrayObjectPtr vertexData,int drawMode, bool isBound){

    bool noShader = Shader::GetCurrentShaderId() == 0;

    #ifndef SILENT_GL
    DisplayGlError("on shader");
    #endif // SILENT_GL

    if (noShader){
        textureShader.Bind();
    }

    ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"g_projection",Projection);

    vertexData->Bind();
    vertexData->SetupVertexes();
    Vertex::SetAttributes();

    if (vertexData->HasElementBuffer()){
        glDrawElements(drawMode, vertexData->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }else{
        glDrawArrays(drawMode, 0, vertexData->GetVertexesCount());
    }

    vertexData->UnBind();

    if (noShader){
        textureShader.Unbind();
    }
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
        case SUPPORT_POINTSPRITE:
            return true;
        default:
            return false;
    }
}

void Painter::SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels ){
    if (!pixels){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Painter::UpdateTexture(BearTexture *tex, PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter){
    if (size.x == 0 || size.y == 0 || tex == nullptr || tex->id == 0){
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, tex->id);
    filter.ApplyFilter();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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

        glGenBuffers(4, Painter::Buffers);

    }
}

#endif // RENDER_OPENGL3
