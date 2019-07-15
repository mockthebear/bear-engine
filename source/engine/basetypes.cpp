#include "basetypes.hpp"
#include "painters/painters.hpp"
#include "../framework/geometry.hpp"


const BearColor BCWhite4[4] = {
    BearColor(),
    BearColor(),
    BearColor(),
    BearColor(),
};

const BearColor BCWhite2[2] = {
    BearColor(),
    BearColor(),
};

void BearTexture::ApplyPixels(uint8_t *pixels){
    if (id != 0){
        Painter::SetTexturePixels(id, PointInt(size_w,size_h), mode, pixels);
    }
};

void BearTexture::ClearTexture(){
    uint32_t tex = DropTexture();
    if (tex > 0)
        glDeleteTextures(1, &tex);
}

uint32_t BearTexture::DropTexture(){
    uint32_t ret = id;
    id = 0;
    return ret;
}

void Vertex::SetAttributes(){
    GLint posAttrib = 0;
    GLint clipAttrib = 1;
    GLint colorAttrib = 2;

    #ifdef NEED_SHADER_LOCATION
    uint32_t shaderId = 0;
    if ((shaderId = Shader::GetCurrentShaderId()) == 0){
        shaderId = Painter::polygonShader.GetId();
    }
    posAttrib = glGetAttribLocation(shaderId, "vPos");
    clipAttrib = glGetAttribLocation(shaderId, "vClip");
    colorAttrib = glGetAttribLocation(shaderId, "vColor");
    #endif // NEED_SHADER_LOCATION

    //Position on shader, size, type, ??, total size, stride (offset)


    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(clipAttrib);
    glVertexAttribPointer(clipAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(4 * sizeof(float)) );


    DisplayGlError("Error on set attribute locations/sizes");
}

void BearException::Show(){
    bear::out << "Exception: "<< Error << "\n";
    bear::out << "Press any key to continue\n";
}
