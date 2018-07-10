#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER

#ifdef RENDER_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include GLES_LIB
#endif // RENDER_OPENGLES

#ifdef RENDER_OPENGL
    #include GL_LIB
#endif // RENDER_OPENGLES
#ifdef RENDER_OPENGL3
    #include GL_LIB
#endif // RENDER_OPENGLES
