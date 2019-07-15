#include "spriteatlas.hpp"
#include "screenmanager.hpp"
#include "collision.hpp"
#include "renderhelp.hpp"
#include "sprite.hpp"


bool SpriteAtlas::Generate(PointInt&& size){
    auto &sm = ScreenManager::GetInstance();
    PointInt maxTsize = sm.GetMaxTextureSize();
    if (size.x > maxTsize.x || size.y > maxTsize.y || size.x == 0 || size.y == 0){
        return false;
    }
    bool success = m_atlas.Generate(size.x, size.y);
    m_size = size;
    m_GridSize = 16;
    m_LowestY = 0;
    m_vertexes = std::make_shared<VertexArrayObject>();
    return success;
}

bool SpriteAtlas::checkValidSpot(RectInt &guessPosition, PointInt &maxLocations){
    bool fine = true;
    for (auto &it : m_sprites){
        if (Collision::IsColliding(it, guessPosition)){
            fine = false;
        }

        maxLocations.x = std::max(maxLocations.x, it.x+it.w);
        maxLocations.y = std::max(maxLocations.y, it.y+it.h);
    }
    return fine;
}

int SpriteAtlas::StichSprite(RenderTexture &rt){
    PointInt size = rt.GetSize();
    if (size.x == 0 || size.y == 0){
        return -1;
    }
    if (!rt.IsLoaded()){
        return -1;
    }
    size.x -= 1;
    size.y -= 1;

    RectInt guessPosition(0.0f, m_LowestY, size.x, size.y);

    if (m_sprites.size() > 0){
        PointInt maxLocations;
        while (!checkValidSpot(guessPosition, maxLocations)){
            /*guessPosition.x++;
            if (guessPosition.x+size.x >= m_size.x){
                guessPosition.x = 0;
                guessPosition.y++;
                if (guessPosition.y+size.y >= m_size.y){
                    return false;
                }
            }*/
            //bear::out << "Failed to add on :" << guessPosition << "\n";
            guessPosition.x += m_GridSize;
            //bear::out << "Trying on :" << guessPosition << "\n";
            if (guessPosition.x+size.x >= m_size.x){
                guessPosition.x = 0;
                guessPosition.y += m_GridSize;

                if (guessPosition.y+size.y >= m_size.y){
                    bear::out << "no space sorry hec\n";
                    return -1;
                }

                if (guessPosition.y > maxLocations.y){
                    //guessPosition.y = maxLocations.y+1;
                }

            }

            if (guessPosition.x > maxLocations.x){
                //bear::out << "Failed to add on :" << guessPosition.x << " X is bigger than: "<<maxLocations<<"\n";
                //guessPosition.x = maxLocations.x+1;
            }
        }
    }

    m_atlas.Bind();
    int mainId = m_sprites.size();
    m_sprites.emplace_back(guessPosition);
    rt.SetPosition(guessPosition.GetPos());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    rt.RenderAll();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_atlas.UnBind();

    //Calculate the lowest Y
    for (int y=0;y<m_size.y;y+=m_GridSize){
        m_LowestY = y - m_GridSize;
        guessPosition = Rect(0.0f, m_LowestY, m_size.x, m_GridSize);
        PointInt maxLocations;
        if (!checkValidSpot(guessPosition, maxLocations)){
            break;
        }
    }
    return mainId;
}

void SpriteAtlas::OpenStich(){
    m_atlas.Bind();
}

void SpriteAtlas::CloseStich(){
    m_atlas.UnBind();
}



int SpriteAtlas::StichSprite(std::string name){
    Sprite tmp(name);
    return StichSprite(tmp);
}
int SpriteAtlas::StichSprite(const char *name){
    Sprite tmp(name);
    return StichSprite(tmp);
}


Sprite SpriteAtlas::SummonSprite(uint32_t spriteId){

    Sprite ret;

    Rect spr = m_sprites[spriteId];
    ret.SetVirtualData(m_size);
    ret.size = Point(spr.w+1, spr.h+1);
    ret.m_renderData.clipOffset.x = spr.x;
    ret.m_renderData.clipOffset.x = spr.x;
    ret.m_renderData.SetClip(Rect(0.0f, 0.0f , spr.w + 1, spr.h + 1), m_size );
    ret.m_vertexes = m_vertexes;

    return ret;
}

bool SpriteAtlas::RenderAll(){
    Painter::RenderTexture(&m_atlas,m_vertexes);
    m_vertexes->clear();
    return true;
}


bool SpriteAtlas::RenderWholeAtlas(PointInt pos){

    m_atlas.Render(pos);
    return true;
}
