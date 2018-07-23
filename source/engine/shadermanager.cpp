#include "shadermanager.hpp"
#include "bear.hpp"
#include <iostream>
#include "../framework/gamefile.hpp"
#include <glm/gtc/type_ptr.hpp>
uint32_t ShaderSetter<std::vector<Rect>>::maxSize = 0;
float *ShaderSetter<std::vector<Rect>>::arr = nullptr;
uint32_t ShaderSetter<std::vector<Point>>::maxSize = 0;
float *ShaderSetter<std::vector<Point>>::arr = nullptr;
uint32_t ShaderSetter<std::vector<Point3>>::maxSize = 0;
float *ShaderSetter<std::vector<Point3>>::arr = nullptr;

GLuint Shader::lastShader = 0;


Shader::Shader(){
    m_shaderId = 0;
}

Shader::Shader(std::string str){
    m_shaderId = 0;
}

void Shader::Close(){
    if (m_shaderId != 0 && glIsProgram(m_shaderId)){
        glDeleteProgram(m_shaderId);
    }
    m_shaderId = 0;
}

bool Shader::Create(){
    if (m_shaderId != 0){
        return false;
    }
    m_shaderId = glCreateProgram();
    return true;
}



bool Shader::LoadShader(int mode,const char * shdr){
    GLint status;

    GLuint thisShader = glCreateShader(mode);
	glShaderSource(thisShader, 1, (const GLchar**)&shdr, NULL );
	glCompileShader(thisShader );
	glGetShaderiv( thisShader, GL_COMPILE_STATUS, &status );

	if(!status){
		ShaderError( thisShader );
		glDeleteShader( thisShader );
        return false;
	}

	glAttachShader( m_shaderId, thisShader );
	glDeleteShader( thisShader );
	return true;
}

bool Shader::Link(){
    GLint status;
    glLinkProgram( m_shaderId );
	glGetProgramiv( m_shaderId, GL_LINK_STATUS, &status );
	if(!status){
		ProgramError( m_shaderId );
        return false;
    };
    return true;
}

bool Shader::Compile(int type,std::string name){

    if( !GLEW_VERSION_2_1 ){
        bear::out << "OpenGL 2.1 not supported!\n";
        return false;
    }

    GameFile file;

    if (!file.Open(name,true)){
        bear::out << "No such file\n";
        return false;
    }

    file.Cache();

    if (m_shaderId == 0){
        if (!Create()){

            bear::out << "Failed to create\n";
            file.Close();
            return false;
        }
    }

    const char* shaderSource = file.GetCache_Unsafe();

    if (!LoadShader(type,shaderSource)){
        file.Close();
        bear::out << "Failed to load: "<<name<< "\n";
        return false;
    }

    file.Close();
    return true;
}

bool Shader::HasUniform(const char *locName){
    return GetUniformLocation(locName) != -1;
}

GLint Shader::GetUniformLocation(const char* locName){

    GLint loc = glGetUniformLocation( m_shaderId, locName );

    return loc;
}

bool Shader::Bind(){

    if (m_shaderId == 0)
        return false;

    glUseProgram( m_shaderId );

    return true;
}
bool Shader::ReBind(){
    glUseProgram(lastShader);
    lastShader = 0;
    return true;
}


bool Shader::Unbind(){
    lastShader = Shader::GetCurrentShaderId();
    glUseProgram(0);
    return true;
}


GLuint Shader::GetCurrentShaderId(){
    int par;
    glGetIntegerv(GL_CURRENT_PROGRAM,&par);
    return par;
}



void Shader::ProgramError( GLuint program ){
	if(glIsProgram( program )){

		int infoLogLength = 0;
		int maxLength = infoLogLength;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
		char* infoLog = new char[ maxLength ];
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );

		if( infoLogLength > 0 ){
			bear::out << "Program error: " << infoLog << "\n";
		}

		delete[] infoLog;
	}
}

void Shader::ShaderError( GLuint shader ){
	if(glIsShader( shader )){

        int infoLogLength = 0;
		int maxLength = infoLogLength;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		char* infoLog = new char[ maxLength ];
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );

		if( infoLogLength > 0 ){
            bear::out << "Shader error: " << infoLog << "\n";
		}

		delete[] infoLog;
	}
}
