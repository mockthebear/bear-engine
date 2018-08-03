#pragma once

#include "libheader.hpp"
#include "../glm/glm.hpp"

#include "basetypes.hpp"
#include "../framework/geometry.hpp"
#include "bear.hpp"
#include <string>
#include <typeinfo>
#include <vector>

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"


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

template<> struct ShaderSetter<std::vector<Point>>{
    static uint32_t maxSize;
    static float *arr;
    static bool SetUniform(GLuint shdr,const char *str,std::vector<Point> var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        if (arr == nullptr || (var.size() * 2) > maxSize){
            arr = new float[var.size() * 2];
            maxSize = var.size() * 2;
        }
        int cnter = 0;
        for (auto &it : var){
            arr[cnter] = it.x;
            arr[cnter+1] = it.y;
            cnter += 2;
        }
        glUniform2fv(loc,var.size(), arr );

        return true;
    };
};

template<> struct ShaderSetter<glm::mat4>{
    static bool SetUniform(GLuint shdr, const char *str, glm::mat4 &data){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(data));
        return true;
    };
};


template<> struct ShaderSetter<std::vector<Point3>>{
    static uint32_t maxSize;
    static float *arr;
    static bool SetUniform(GLuint shdr,const char *str,std::vector<Point3> var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        if (arr == nullptr || (var.size() * 3) > maxSize){
            arr = new float[var.size() * 3];
            maxSize = var.size() * 3;
        }
        int cnter = 0;
        for (auto &it : var){
            arr[cnter] = it.x;
            arr[cnter+1] = it.y;
            arr[cnter+2] = it.z;
            cnter += 3;
        }
        glUniform3fv(loc,var.size(), arr );
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

template<> struct ShaderSetter<std::vector<Rect>>{
    static uint32_t maxSize;
    static float *arr;
    static bool SetUniform(GLuint shdr,const char *str,std::vector<Rect> var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        if (arr == nullptr || (var.size() * 4) > maxSize){
            arr = new float[var.size() * 4];
            maxSize = var.size() * 4;
        }

        int cnter = 0;
        for (auto &it : var){
            arr[cnter] = it.x;
            arr[cnter+1] = it.y;
            arr[cnter+2] = it.w;
            arr[cnter+3] = it.h;
            cnter += 4;
        }
        glUniform4fv(loc,var.size(), arr );
        return true;
    };
};

template<> struct ShaderSetter<TexturePtr>{
    static bool SetUniform(GLuint shdr,const char *str,TexturePtr tex,int id = 0){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1 || !tex.get() || tex.get()->id == 0){
            return false;
        }
        glUniform1i(loc,id);
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, tex.get()->id);
        return true;
    };
    static bool DisableTexture(int id = 0){
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
};

template<> struct ShaderSetter<BearTexture>{
    static bool SetUniform(GLuint shdr,const char *str,BearTexture& tex,int id = 0){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1 || tex.id == 0){
            return false;
        }
        glUniform1i(loc,id);
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        return true;
    };
    static bool DisableTexture(int id = 0){
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
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

template<> struct ShaderSetter<BearColor>{
    static bool SetUniform(GLuint shdr,const char *str,BearColor var){
        GLint loc = glGetUniformLocation(shdr, str );
        if (loc == -1){
            return false;
        }
        glUniform4f(loc,var.r,var.g,var.b,var.a);
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
        bool LoadShader(int type,const char * shdr);
        bool Link();

        bool CompileFromString(int type,const char * shdr);
        bool Compile(int type,std::string fragmentFilePath);
        bool Bind();


        GLint GetUniformLocation(const char* locName);
        bool HasUniform(const char *locName);


        template <typename T> bool SetUniform(const char *str,T var){
            return ShaderSetter<T>::SetUniform(m_shaderId,str,var);
        };
        static bool Unbind();
        static bool ReBind();
        static GLuint GetCurrentShaderId();
        static GLuint lastShader;

        uint32_t GetId(){return m_shaderId;};


        static const char *DefaultTextureVertexShader;
        static const char *DefaultTextureFragmentShader;

        static const char *DefaultQuadVertexShader;
        static const char *DefaultQuadFragmentShader;


    private:

        void ProgramError( GLuint program );
        void ShaderError( GLuint shader );

        GLuint m_shaderId;

};
