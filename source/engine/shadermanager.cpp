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
        return false;
	}

	glAttachShader( m_shaderId, fragmentShader );
    GLint programSuccess;


    glLinkProgram( m_shaderId );

	glGetProgramiv( m_shaderId, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
    {
		printf( "Error linking program %d!\n", m_shaderId );
        return false;
    };

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );



    mVertexPos2DLocation = glGetAttribLocation( m_shaderId, "LVertexPos2D" );
    if( mVertexPos2DLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LVertexPos2D" );
    }

    mTexCoordLocation = glGetAttribLocation( m_shaderId, "LTexCoord" );
    if( mTexCoordLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LTexCoord" );
    }

    mTextureColorLocation = glGetUniformLocation( m_shaderId, "LTextureColor" );
    if( mTextureColorLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LTextureColor" );
    }

    mTextureUnitLocation = glGetUniformLocation( m_shaderId, "LTextureUnit" );
    if( mTextureUnitLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LTextureUnit" );
    }

    mProjectionMatrixLocation = glGetUniformLocation( m_shaderId, "LProjectionMatrix" );
    if( mProjectionMatrixLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LProjectionMatrix" );
    }

    mModelViewMatrixLocation = glGetUniformLocation( m_shaderId, "LModelViewMatrix" );
    if( mModelViewMatrixLocation == -1 )
    {
        printf( "%s is not a valid glsl program variable!\n", "LModelViewMatrix" );
    }




    file_vert.Close();
    file_frag.Close();
    return true;
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



void Shader::setVertexPointer( GLsizei stride, const GLvoid* data )
{
    glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void Shader::setTexCoordPointer( GLsizei stride, const GLvoid* data )
{
    glVertexAttribPointer( mTexCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void Shader::enableVertexPointer()
{
    glEnableVertexAttribArray( mVertexPos2DLocation );
}

void Shader::disableVertexPointer()
{
    glDisableVertexAttribArray( mVertexPos2DLocation );
}

void Shader::enableTexCoordPointer()
{
    glEnableVertexAttribArray( mTexCoordLocation );
}

void Shader::disableTexCoordPointer()
{
    glDisableVertexAttribArray( mTexCoordLocation );
}

void Shader::setProjection( glm::mat4 matrix )
{
    mProjectionMatrix = matrix;
}

void Shader::setModelView( glm::mat4 matrix )
{
    mModelViewMatrix = matrix;
}

void Shader::leftMultProjection( glm::mat4 matrix )
{
    mProjectionMatrix = matrix * mProjectionMatrix;
}

void Shader::leftMultModelView( glm::mat4 matrix )
{
    mModelViewMatrix = matrix * mModelViewMatrix;
}

void Shader::updateProjection()
{
    glUniformMatrix4fv( mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
}

void Shader::updateModelView()
{
    glUniformMatrix4fv( mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
}

void Shader::setTextureColor( LColorRGBA color )
{
    glUniform4fv( mTextureColorLocation, 4, (const GLfloat*)&color );
}

void Shader::setTextureUnit( GLuint unit )
{
    glUniform1i( mTextureUnitLocation, unit );
}
