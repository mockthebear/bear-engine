#pragma once

#include "../../settings/definitions.hpp"
#include "../../framework/geometry.hpp"
#include "../../framework/chainptr.hpp"


#include SDL_LIB_HEADER
#include "../libheader.hpp"


#include "../basetypes.hpp"
#include "../shadermanager.hpp"
#include "../../framework/vertex.hpp"
#include <memory>
#include <iostream>

#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/type_ptr.hpp"


class ScreenManager;
class Vertex;
class Painter;


class Painter{
  public:
    //static glm::mat4 CalculateModel(BasicRenderDataPtr );

    static void SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels = nullptr );

    static void SetViewport(Point screenNow,Point offset = Point(0.0f,0.0f));
    static void SetProjection(Point size,int flipScreen = SDL_FLIP_NONE);
    static void SetProjection(Rect rproj);

    static void SetEmptyTexture();

    static glm::mat4 Projection;


    static bool RenderTexture(BearTexture *ptr,VertexArrayObjectPtr vertexData);
    /**
        *Make a render call.
        @param vertexes A reference to a VertexArrayObject. It carry all the vertexes informations
        @param renderInfo A reference to a BasicRenderData. It carry some more information like transformations passed to shader or model things
        @param drawMode Base is GL_TRIANGLES. But do your thing
        @param isBound this avoid double bound of a vertexes param. Use as true in case of you bound previously.
    */
    static void DrawVertex(VertexArrayObjectPtr vertexes,int drawMode = GL_TRIANGLES,bool isBound = false);


    static bool DrawSprites(int id);


    static bool UpdateTexture(BearTexture *tex, PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter);

    static BearTexture * MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter);

     static bool CanSupport(PainterSupport sup);


    static uint32_t powerOfTwo( uint32_t num ){
        if( num != 0 ){
            if ((num & (num - 1)) == 0){
                return num;
            }
            num--;
            num |= (num >> 1);
            num |= (num >> 2);
            num |= (num >> 4);
            num |= (num >> 8);
            num |= (num >> 16);
            num++;
        }
        return num;
    }
    static uint32_t GetSharedBuffer(int id);

    static Shader textureShader;
    static Shader pointTextureShader;
    static Shader polygonShader;

    static uint32_t UnloadedTextureId;
    static BearTexture *UnloadedTexture;
    static TexturePtr UnloadedTexturePtr;

  private:
    friend class ScreenManager;


    static bool SetupEnvoriment(ScreenManager *sm);
    static int GetMaxTextureSize();


    static void SetupShaders();




    static bool m_shaderBuilt;


    static VertexArrayObject m_vao;

    static uint32_t Buffers[4];
};
