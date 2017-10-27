#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

struct LColorRGBA
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};


struct LTexCoord
{
    GLfloat s;
    GLfloat t;
};

class Shader{
    public:
        void printProgramLog( GLuint program );
        void printShaderLog( GLuint shader );
        Shader();
        Shader(std::string file);
        void Close();

        bool Compile(std::string vertexFilePath,std::string fragmentFilePath);
        bool Bind();
        bool Unbind();

        GLint GetUniformLocation(const char* locName);

    private:
        GLuint m_shaderId;



};


extern Shader g_shader;
