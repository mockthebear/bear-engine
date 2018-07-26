#include "pstileset.hpp"
#include "painters/painters.hpp"
#include "screenmanager.hpp"
#include "renderhelp.hpp"


PointSpriteTileset::PointSpriteTileset(){
    float points[] ={
        32.0f,  1.0f,  0.0f, // top-left
        32.0f,  32.0f,  10.0f, // top-right
        32.0f,  64.0f,  20.0f, // bottom-right
        32.0f,  96.0f,  30.0f,  // bottom-left
        0.0f,  96.0f, 173.0f,  // bottom-left
    };
    uint32_t indices[] = {
        0, 1, 2, 3,
    };
    std::cout << sizeof(points) << "\n";

    m_vao.DisableElements();

    m_vao.vertexes.AddVertexes( sizeof(points) / sizeof(float) ,points);

    m_vao.vertexes.AddIndices( 4,indices);



    m_vao.SetupVertexes(false);

    //m_vao.Bind();

    //glBufferData(GL_ARRAY_BUFFER,  sizeof(float), &points, GL_DYNAMIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(uint32_t), &indices, GL_STATIC_DRAW);



    /*glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    m_vao.UnBind();*/


    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,  m_vao.vertexes.vertexData.size() * sizeof(float), &m_vao.vertexes.vertexData[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);


    m_psShader.Compile(GL_VERTEX_SHADER, "data/shaders/ps_vertex.glfs");
    m_psShader.Compile(GL_GEOMETRY_SHADER, "data/shaders/ps_geometry.glfs");
    m_psShader.Compile(GL_FRAGMENT_SHADER, "data/shaders/ps_frag.glfs");
    m_psShader.Link();

    m_textureAtlas = Sprite("data/tiles.png");
}

void PointSpriteTileset::Render(){

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //ShaderSetter<glm::mat4>::SetUniform(Shader::GetCurrentShaderId(),"projection",Painter::Projection);

    m_psShader.Bind();


    ShaderSetter<int>::SetUniform(Shader::GetCurrentShaderId(),"image",0);
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"tileSize",Point(32,32));
    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"setSize",m_textureAtlas.GetSize());

    Point sz = ScreenManager::GetInstance().GetGameSize();



    sz.x = (1.0f / sz.x) * 2.0;  //2.0 because its from -1 to 1.
    sz.y = (1.0f / sz.y) * 2.0;  //2.0 because its from -1 to 1.

    ShaderSetter<Point>::SetUniform(Shader::GetCurrentShaderId(),"orthoVec",sz);



    glBindTexture( GL_TEXTURE_2D, m_textureAtlas.GetTexture().get()->id );
    //m_vao.Bind();
     //glDrawElements(GL_QUADS, m_vao.GetIndexCount(), GL_UNSIGNED_INT, 0);

    //glDrawArrays(GL_POINTS, 0, 4);
    //glPointSize(8.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 5);

    m_psShader.Unbind();

    //RenderHelp::DrawSquareColor(Rect(32,32,32,32),255,255,255,255);



    //m_vao.UnBind();


}
