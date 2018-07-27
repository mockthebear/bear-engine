#include "pstileset.hpp"
#include "painters/painters.hpp"
#include "screenmanager.hpp"
#include "renderhelp.hpp"

#define VERTEX_ELEMENTCOUNT 3

PointSpriteTileset::PointSpriteTileset(){
    if (Painter::CanSupport(SUPPORT_POINTSPRITE)){
        #ifdef SUPPORT_GEOMETRY_SHADER
        m_vao.DisableElements();

        m_vao.SetupVertexes(false);

        m_vao.Bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_ELEMENTCOUNT * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, VERTEX_ELEMENTCOUNT * sizeof(float), (void*)(2 * sizeof(float)));
        m_vao.UnBind();


        m_psShader.Compile(GL_VERTEX_SHADER, "data/shaders/ps_vertex.glfs");
        m_psShader.Compile(GL_GEOMETRY_SHADER, "data/shaders/ps_geometry.glfs");
        m_psShader.Compile(GL_FRAGMENT_SHADER, "data/shaders/ps_frag.glfs");
        m_psShader.Link();

        m_textureAtlas = Sprite("data/tiles.png");
        #endif // SUPPORT_GEOMETRY_SHADER
    }else{
        bear::out << "Point sprite is not supported!\n";
    }
}

void PointSpriteTileset::AddTile(Point pos, int id){
    if (Painter::CanSupport(SUPPORT_POINTSPRITE)){
        float points[] ={
            pos.x,  pos.y,  float(id),
        };
        m_vao.vertexes.AddVertexes( sizeof(points) / sizeof(float) ,points);
        m_needBufferUpdate = true;
    }
}

void PointSpriteTileset::Render(){
    if (Painter::CanSupport(SUPPORT_POINTSPRITE)){
        if (m_needBufferUpdate){
            m_vao.SetupVertexes(false);
            m_needBufferUpdate = false;
        }
        m_psShader.Bind();
        ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);
        ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"tileSize",Point(32,32));
        ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"setSize",m_textureAtlas.GetSize());
        Point sz = ScreenManager::GetInstance().GetGameSize();
        sz.x = (1.0f / sz.x) * 2.0;  //2.0 because its from -1 to 1.
        sz.y = (1.0f / sz.y) * 2.0;  //2.0 because its from -1 to 1.
        ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"orthoVec",sz);

        m_vao.Bind();
        glBindTexture( GL_TEXTURE_2D, m_textureAtlas.GetTexture().get()->id );
        glDrawArrays(GL_POINTS, 0, m_vao.GetVertexesCount()/VERTEX_ELEMENTCOUNT);
        m_psShader.Unbind();
        m_vao.UnBind();
    }else{
        bear::out << "Point sprite is not supported!\n";
    }
}
