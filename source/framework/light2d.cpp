#include "light2d.hpp"
#include "../engine/screenmanager.hpp"


Light2D::~Light2D(){

}


void Light2D::Start(PointInt size, float maxDark){
    m_maxDark = maxDark;
    m_sizeMultiplier = 1.0f;
    m_size = size / m_sizeMultiplier;
    m_targetTexture.textureMode = TEXTURE_TRILINEAR;
    m_targetTexture.Generate(size.x / m_sizeMultiplier, size.y / m_sizeMultiplier);

    m_haloShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultQuadVertexShader);
    m_haloShader.Compile(GL_FRAGMENT_SHADER, "engine/shadow.glfs");
    m_haloShader.Link();

    m_invertShader.CompileFromString(GL_VERTEX_SHADER, Shader::DefaultTextureVertexShader);
    m_invertShader.Compile(GL_FRAGMENT_SHADER, "engine/shadow-inverter.glfs");
    m_invertShader.Link();
}

void Light2D::StoreOpaque(Rect objBox){
    objBox.x = objBox.x / m_sizeMultiplier;
    objBox.y = objBox.y / m_sizeMultiplier;
    objBox.h = objBox.h / m_sizeMultiplier;
    objBox.w = objBox.w / m_sizeMultiplier;
    m_opaqueObjs.emplace_back(objBox);
}

void Light2D::ClearOpaque(){
    m_opaqueObjs.clear();
}

void Light2D::SetLight(Point pos, float radius, BearColor color, float minRays){
    std::vector<Rect> checkObjs;
    std::vector<LineData> actualLines;

    radius = radius/ m_sizeMultiplier;

    pos.x = pos.x/ m_sizeMultiplier;
    pos.y = pos.y/ m_sizeMultiplier;

    Circle checkCircle(pos.x, pos.y, radius);
    for (auto &it : m_opaqueObjs){
        if (Collision::IsColliding(it, checkCircle)){
            checkObjs.emplace_back(it);
        }
    }


    for (float i=0.0f;i<minRays;++i){
        float radAngle = Geometry::toRad( ((360.0f / minRays) * i ) + 45.0 );
        Line currentLine(pos.x   ,   pos.y,  pos.x - radius * cos(radAngle) ,   pos.y - radius * sin(radAngle) );
        actualLines.emplace_back( LineData( currentLine , radAngle) );
    }
    /*
        Scan for vetices and add them as lines without intersecting (trying)
    */

    for (auto &it : checkObjs){
        auto&& checkPoints    = it.Points();
        for (auto &squareVert : checkPoints){
            float dis1 = pos.GetDistance(squareVert);
            dis1 = std::min(radius, dis1  );
            float dir1 = squareVert.GetDirection(pos);
            dir1 = Geometry::AlignRad(dir1);
            actualLines.emplace_back( LineData( Line(pos.x   ,   pos.y, pos.x - dis1 * cos(dir1) ,   pos.y - dis1 * sin(dir1)), dir1) );
            dir1 = Geometry::AlignRad(dir1 + 0.001f);
            actualLines.emplace_back( LineData( Line(pos.x   ,   pos.y, pos.x - radius * cos(dir1) ,   pos.y - radius * sin(dir1)), dir1) );
            dir1 = Geometry::AlignRad(dir1 - 0.002f);
            actualLines.emplace_back( LineData( Line(pos.x   ,   pos.y, pos.x - radius * cos(dir1) ,   pos.y - radius * sin(dir1)), dir1) );
        }
    }

    /*
        Crip lines by any intersection found
    */
    for (auto &actualLine : actualLines){
        float distance = radius;
        for (auto &it : checkObjs){
            auto&& checkLines     = it.Lines();
            for (auto &squareLine : checkLines){
                Point p =  actualLine.line.GetIntersection(squareLine);
                if (p.IsValid()){
                    distance = std::min(distance, pos.GetDistance(p)  );
                    actualLine.line.x2 = pos.x - distance * cos(actualLine.rot);
                    actualLine.line.y2 = pos.y - distance * sin(actualLine.rot);
                }
            }
        }
    }
    utils::bsort<LineData>( &actualLines[0], actualLines.size() );

    RenderLineData(actualLines, pos, radius, color);
}
void Light2D::ClearCanvas(){
    m_targetTexture.Bind();
        m_targetTexture.Clear();
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        RenderHelp::DrawSquareColor(RectColor(0,0, m_size.x, m_size.y, 0, 0, 0), (1.0f - m_maxDark) * 255.0f );
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_targetTexture.UnBind();
}

void Light2D::RenderLineData(std::vector<LineData> &ld, Point pos, float radius, BearColor color){
   /* m_targetTexture.Bind();
        //glBlendEquation(GL_FUNC_ADD);
        glBlendEquation(GL_MAX);
        m_haloShader.Bind();
            m_haloShader.SetUniform<Point>("center", pos );
            m_haloShader.SetUniform<float>("maxDark", m_maxDark );
            m_haloShader.SetUniform<Point>("screenSize", ScreenManager::GetInstance().GetGameSize() );
            m_haloShader.SetUniform<float>("radius", radius );

            static VertexArrayObjectPtr vertexBuffer     = std::make_shared<VertexArrayObject>();
            static BasicRenderDataPtr renderData         = std::make_shared<BasicRenderData>();
            renderData->color = color;

            Circle c(pos.x,pos.y,radius);
            renderData->position = pos;
            renderData->size.x = radius;
            renderData->size.y = radius;
            vertexBuffer->clear();
            Vertex lastP;
            vertexBuffer->AddVertice(Vertex(pos, BearColor(1.0f, 1.0f, 0.0f, 1.0f) ));
            bool first = true;
            Point fir;
            for (auto &it : ld){
                Point p2 = it.line.GetSecond();
                vertexBuffer->AddVertice(Vertex(p2, BearColor(1.0f, 1.0f, 0.0f, 1.0f) ));
                if (first){
                    fir = p2;
                    first = false;
                }
            }
            vertexBuffer->AddVertice(Vertex(fir, BearColor(1.0f, 1.0f, 0.0f, 1.0f) ));
            vertexBuffer->SetupVertexes();

            Painter::DrawVertex(vertexBuffer,renderData,GL_TRIANGLE_FAN, GL_IS_AUTOBOUND);
        m_haloShader.Unbind();
    m_targetTexture.UnBind();
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
}

void Light2D::Render(Point pos){
    m_invertShader.Bind();
        m_invertShader.SetUniform<float>("maxDark", m_maxDark );

        m_targetTexture.SetScale(Point(m_sizeMultiplier, m_sizeMultiplier));
        m_targetTexture.Render(pos);
    m_invertShader.Unbind();
}
