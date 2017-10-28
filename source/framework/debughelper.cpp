#include "debughelper.hpp"
#include "../engine/bear.hpp"
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>

void DebugHelper::AssertAlloc(void *ptr,std::string file,std::string func,int line,std::string msg){
    if (ptr == NULL){
        bear::out << "Assertion error: 0x0 on " << func << ":"<<line<<"\n\n\n"<<file<<"\n\n-------"<<msg<<"\n";
        getchar();
    }
}
void DebugHelper::DisplayGlError(){
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        bear::out << "OpenGL error: " << (const char*)gluErrorString( error ) <<"\n";
    }
}
