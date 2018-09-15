#include "shadermanager.hpp"

#if defined(RENDER_OPENGL3)

const char *Shader::DefaultTextureVertexShader = "\n"
"#version 130\n"
"in vec2 vPos;\n"
"in vec2 clip;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"uniform vec2 translation;\n"
"\n"
"void main()\n"
"{\n"
"    TexCoords = clip;\n"
"    gl_Position = projection * vec4(vPos.xy + translation, 0.0, 1.0);\n"
"}\n";

const char *Shader::DefaultTextureFragmentShader = "\n"
"#version 130\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D image;\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(image, TexCoords) * iColor;\n"
"\n"
"}\n";


const char *Shader::DefaultQuadVertexShader = "\n"
"#version 130\n"
"in vec2 vPos;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vPos.xy, 0.0, 1.0);\n"
"}\n";

const char *Shader::DefaultQuadFragmentShader = "\n"
"#version 130\n"
"out vec4 color;\n"
"\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    color = iColor;\n"
"\n"
"}\n";

#elif defined(RENDER_OPENGL2)


const char *Shader::DefaultTextureVertexShader = "\n"
"#version 100\n"
"attribute vec2 vPos;\n"
"attribute vec2 clip;\n"
"\n"
"varying  vec2 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"   TexCoords = clip;\n"
"   gl_Position = projection * vec4(vPos.xy, 0.0, 1.0);\n"
"}\n";

const char *Shader::DefaultTextureFragmentShader = "\n"
"#version 100\n"
"precision mediump float;\n"
"\n"
"varying vec2 TexCoords;\n"
"\n"
"uniform sampler2D image;\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor  = texture2D(image, TexCoords) * iColor;\n"
"\n"
"}\n";


const char *Shader::DefaultQuadVertexShader = "\n"
"#version 100\n"
"attribute vec2 vPos;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vPos.xy, 0.0, 1.0);\n"
"}\n";

const char *Shader::DefaultQuadFragmentShader = "\n"
"#version 100\n"
"precision mediump float;\n"
"\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = iColor;\n"
"\n"
"}\n";


#elif defined(RENDER_OPENGLES2)

const char *Shader::DefaultTextureVertexShader = "\n"
"#version 300 core\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec2 clip;\n"
"\n"
"out vec2 TexCoords;a\n"
"void main()\n"
"{\n"
"    TexCoords = gl_Position;\n"
"}\n";

const char *Shader::DefaultTextureFragmentShader = "\n"
"#version 130 core\n"
"in vec2 TexCoords;\n"
"layout(location = 0) out vec4 color;\n"
"\n"
"uniform sampler2D image;\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(image, TexCoords) * iColor;\n"
"\n"
"}\n";


const char *Shader::DefaultQuadVertexShader = "\n"
"#version 130 core\n"
"layout (location = 0) in vec2 vPos;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * model * vec4(vPos.xy, 0.0, 1.0);\n"
"}\n";

const char *Shader::DefaultQuadFragmentShader = "\n"
"#version 130 core\n"
"layout(location = 0) out vec4 color;\n"
"\n"
"uniform vec4 iColor;\n"
"\n"
"void main()\n"
"{\n"
"    color = iColor;\n"
"\n"
"}\n";


#elif defined(RENDER_OPENGL)



const char *Shader::DefaultTextureVertexShader = "\n"
"\n"
"varying vec2 TexCoords;\n"
"\n"
"void main()\n"
"{\n"
"    TexCoords[0] = gl_MultiTexCoord0[0];\n"
"    TexCoords[1] = gl_MultiTexCoord0[1];\n"
"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const char *Shader::DefaultTextureFragmentShader = "\n"
"\n"
"uniform sampler2D image;\n"
"uniform vec4 iColor;\n"
"in vec2 TexCoords;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(image, TexCoords) * iColor; \n"
"\n"
"}\n";


const char *Shader::DefaultQuadVertexShader = "\n"
"#version 130 core\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"void main()\n"
"{\n"
"    TexCoords[0] = gl_MultiTexCoord0[0];\n"
"    TexCoords[1] = gl_MultiTexCoord0[1];\n"
"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const char *Shader::DefaultQuadFragmentShader = "\n"
"#version 130 core\n"
"in vec2 TexCoords;\n"
"\n"
"uniform sampler2D image;\n"
"uniform vec4 iColor;\n"
"in vec2 TexCoords;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(image, TexCoords) * iColor\n"
"\n"
"}\n";


#endif // defined
