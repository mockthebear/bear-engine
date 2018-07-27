#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER

#ifdef RENDER_OPENGLES2
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
    #include GL_LIB
    #define SUPPORT_GLEW
#endif // RENDER_OPENGL
#ifdef RENDER_OPENGL3
    #include GL_LIB
    #define SUPPORT_GEOMETRY_SHADER
    #define SUPPORT_GLEW
#endif // RENDER_OPENGL3


