#include "debughelper.hpp"
#include "../engine/bear.hpp"
#include <iostream>
#include <stdio.h>

#ifdef RENDER_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include GLES_LIB
#endif // RENDER_OPENGLES

#ifdef RENDER_OPENGL
    #include GL_LIB
#endif // RENDER_OPENGLES


void DebugHelper::AssertAlloc(void *ptr,std::string file,std::string func,int line,std::string msg){
    if (ptr == NULL){
        bear::out << "Assertion error: 0x0 on " << func << ":"<<line<<"\n\n\n"<<file<<"\n\n-------"<<msg<<"\n";
        getchar();
    }
}
void DebugHelper::DisplayGlError(std::string ate){
    GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::string name = "??????";
		if (err == GL_INVALID_ENUM)
			name = "GL_INVALID_ENUM";

		if (err == GL_INVALID_VALUE)
			name = "GL_INVALID_VALUE";

		if (err == GL_INVALID_OPERATION)
			name = "GL_INVALID_OPERATION";

		if (err == GL_INVALID_FRAMEBUFFER_OPERATION)
			name = "GL_INVALID_FRAMEBUFFER_OPERATION";

		if (err == GL_OUT_OF_MEMORY)
			name = "GL_OUT_OF_MEMORY";


		if (err == GL_STACK_UNDERFLOW)
			name = "GL_STACK_UNDERFLOW";

		if (err == GL_STACK_OVERFLOW)
			name = "GL_STACK_OVERFLOW";

		bear::out << "GL error: " << name << " at "<<ate<<"\n";
	}
}
