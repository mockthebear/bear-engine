#pragma once
#include "../settings/definitions.hpp"
#ifdef RENDER_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include GLES_LIB
#elif RENDER_OPENGL
    #include GL_LIB
#endif // RENDER_OPENGLES

#include <glm/glm.hpp>

#include "../framework/geometry.hpp"
#include "bear.hpp"
#include <string>
#include <typeinfo>

template <typename T> struct ShaderSetter{
     static bool SetUniform(GLuint shdr,const char *str,T var){
        bear::out << "Undefined "<<typeid(T).name()<<"\n";
        return false;
    };
};

template<> struct ShaderSetter<Point>{
    static bool SetUniform(GLuint shdr,const char *str,Point var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform2f(loc,var.x,var.y);
        return true;
    };
};

template<> struct ShaderSetter<Point3>{
    static bool SetUniform(GLuint shdr,const char *str,Point3 var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform3f(loc,var.x,var.y,var.y);
        return true;
    };
};

template<> struct ShaderSetter<Rect>{
    static bool SetUniform(GLuint shdr,const char *str,Rect var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform4f(loc,var.x,var.y,var.w,var.h);
        return true;
    };
};

template<> struct ShaderSetter<int>{
    static bool SetUniform(GLuint shdr,const char *str,int var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform1i(loc,var);
        return true;
    };
};

template<> struct ShaderSetter<float>{
    static bool SetUniform(GLuint shdr,const char *str,float var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform1f(loc,var);
        return true;
    };
};

template<> struct ShaderSetter<SDL_Color>{
    static bool SetUniform(GLuint shdr,const char *str,SDL_Color var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform4f(loc,var.r/255.0f,var.g/255.0f,var.b/255.0f,var.a/255.0f);
        return true;
    };
};

class Shader{
    public:

        Shader();
        Shader(std::string file);

        bool IsLoaded(){return m_shaderId > 0;};
        void Close();

        bool Create();
        bool LoadVertexShader(const char * shdr);
        bool LoadFragmentShader(const char * shdr);
        bool Link();

        bool Compile(std::string vertexFilePath,std::string fragmentFilePath);
        bool Bind();
        bool Unbind();

        GLint GetUniformLocation(const char* locName);
        bool HasUniform(const char *locName);


        template <typename T> bool SetUniform(const char *str,T var){
            return ShaderSetter<T>::SetUniform(m_shaderId,str,var);
        };


    private:

        void ProgramError( GLuint program );
        void ShaderError( GLuint shader );

        GLuint m_shaderId,m_fragmentShader,m_vertexShader;

};

