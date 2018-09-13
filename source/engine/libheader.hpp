#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER
#define GL_LIB <GL/glew.h>

#ifdef RENDER_OPENGLES2
    #define GL_IS_AUTOBOUND true
    #ifdef __EMSCRIPTEN__
        #include <emscripten.h>
        #define GL_GLEXT_PROTOTYPES 1
        #include <SDL_opengles2.h>
    #else
        #define GL_GLEXT_PROTOTYPES 1
        #include <SDL2/SDL_opengles2.h>
    #endif
#endif // RENDER_OPENGLES2

#ifdef RENDER_OPENGL
    #define GL_IS_AUTOBOUND true
    #include GL_LIB
    #define SUPPORT_GLEW
#endif // RENDER_OPENGL
#ifdef RENDER_OPENGL2
    #define GL_IS_AUTOBOUND true
    #include GL_LIB
    #define SUPPORT_VERTEX_BUFFER
    //#define SUPPORT_SINGLE_BUFFER
    //#define REMAKE_VETEX_ON_BIND
    #define SUPPORT_GLEW
#endif // RENDER_OPENGL2
#ifdef RENDER_OPENGL3
    #define GL_IS_AUTOBOUND false
    #include GL_LIB
    #define SUPPORT_GEOMETRY_SHADER
    #define SUPPORT_VERTEX_ARRAY
    #define SUPPORT_VERTEX_BUFFER
    #define SUPPORT_GLEW
#endif // RENDER_OPENGL3


