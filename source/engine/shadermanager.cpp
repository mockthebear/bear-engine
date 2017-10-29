#include "shadermanager.hpp"
#include "bear.hpp"
#include <iostream>
#include "../framework/gamefile.hpp"
#include <glm/gtc/type_ptr.hpp>

Shader g_shader;

Shader::Shader(){
    m_shaderId = 0;
}


Shader::Shader(std::string str){
    m_shaderId = 0;
}


void Shader::Close(){
    if (m_shaderId != 0){
        glDeleteProgram(m_shaderId);
    }
    m_shaderId = 0;
}


void Shader::printProgramLog( GLuint program )
{
	//Make sure name is shader
	if( glIsProgram( program ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			printf( "%s\n", infoLog );
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a program\n", program );
	}
}

void Shader::printShaderLog( GLuint shader )
{
	//Make sure name is shader
	if( glIsShader( shader ) )
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			printf( "%s\n", infoLog );
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a shader\n", shader );
	}
}


bool Shader::Compile(std::string vert,std::string frag){
    if( !GLEW_VERSION_2_1 ){
        bear::out << "OpenGL 2.1 not supported!\n" << "\n";
        return false;
    }
    GameFile file_vert;
    GameFile file_frag;
    bear::out << "Loading\n";
    if (!file_vert.Open(vert,true)){
        return false;
    }
    if (!file_frag.Open(frag,true)){
        file_vert.Close();
        return false;
    }

    file_vert.Cache();
    file_frag.Cache();

    bear::out << "Cached\n";

    m_shaderId = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

	//Get vertex source

    const GLchar* shaderSource = file_vert.GetCache_Unsafe();

	glShaderSource( vertexShader, 1, (const GLchar**)&shaderSource, NULL );
    bear::out << "buil\n";
	glCompileShader( vertexShader );
	bear::out << "gg\n";

	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
	if( vShaderCompiled != GL_TRUE )
	{
		printf( "Unable to compile vertex shader %d!\n", vertexShader );
		printShaderLog( vertexShader );
        return false;
	}

	glAttachShader( m_shaderId, vertexShader );

	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

	const GLchar* shaderSource2 = file_frag.GetCache_Unsafe();

	glShaderSource( fragmentShader, 1, (const GLchar**)&shaderSource2, NULL );

	glCompileShader( fragmentShader );

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
	if( fShaderCompiled != GL_TRUE )
	{
		printf( "Unable to compile fragment shader %d!\n", fragmentShader );
		printShaderLog( fragmentShader );
        return false;
	}

	glAttachShader( m_shaderId, fragmentShader );
    GLint programSuccess;


    glLinkProgram( m_shaderId );

	glGetProgramiv( m_shaderId, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
    {
		printf( "Error linking program %d!\n", m_shaderId );
		printProgramLog( m_shaderId );
        return false;
    };

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );






    file_vert.Close();
    file_frag.Close();
    return true;
}

GLint Shader::GetUniformLocation(const char* locName){
    GLint loc = glGetUniformLocation( m_shaderId, locName );
    if( loc == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", locName );
        getchar();
    }
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


