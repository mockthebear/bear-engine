#include "shadermanager.hpp"
#include "bear.hpp"
#include <iostream>
#include "../framework/gamefile.hpp"
#include <glm/gtc/type_ptr.hpp>

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

bool Shader::LoadVertexShader(const char * shdr){
    GLint status;
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertexShader, 1, (const GLchar**)&shdr, NULL );
	glCompileShader(m_vertexShader );
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status );
	if(!status){
		ShaderError(m_vertexShader);
		glDeleteShader( m_vertexShader );
        return false;
	}
	glAttachShader(m_shaderId, m_vertexShader);
	glDeleteShader( m_vertexShader );
	m_vertexShader = 0;
	return true;
}

bool Shader::LoadFragmentShader(const char * shdr){
    GLint status;
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragmentShader, 1, (const GLchar**)&shdr, NULL );
	glCompileShader(m_fragmentShader );
	glGetShaderiv( m_fragmentShader, GL_COMPILE_STATUS, &status );

	if(!status){
		ShaderError( m_fragmentShader );
		glDeleteShader( m_fragmentShader );
        return false;
	}

	glAttachShader( m_shaderId, m_fragmentShader );
	glDeleteShader( m_fragmentShader );
	m_fragmentShader = 0;
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

bool Shader::Compile(std::string vert,std::string frag){

    /*if( !GLEW_VERSION_2_1 ){
        bear::out << "OpenGL 2.1 not supported!\n";
        return false;
    }*/

    GameFile file_vert;
    GameFile file_frag;

    if (!file_vert.Open(vert,true)){
        return false;
    }

    if (!file_frag.Open(frag,true)){
        file_vert.Close();
        return false;
    }

    file_vert.Cache();
    file_frag.Cache();

    if (!Create()){
        file_frag.Close();
        file_vert.Close();
        return false;
    }

    const char* shaderSource = file_vert.GetCache_Unsafe();

    if (!LoadVertexShader(shaderSource)){
        file_frag.Close();
        file_vert.Close();
        return false;
    }

    const char* shaderSource2 = file_frag.GetCache_Unsafe();

    if (!LoadFragmentShader(shaderSource2)){
        file_frag.Close();
        file_vert.Close();
        return false;
    }

    if (!Link()){
        file_frag.Close();
        file_vert.Close();
        return false;
    }

    file_vert.Close();
    file_frag.Close();

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
bool Shader::Unbind(){
    glUseProgram(0);
    return true;
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




