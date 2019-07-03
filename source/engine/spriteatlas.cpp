#include "spriteatlas.hpp"
#include "screenmanager.hpp"


bool SpriteAtlas::Generate(PointInt&& size){
    /*auto &sm = ScreenManager::GetInstance();
    PointInt &maxTsize = sm.GetMaxTextureSize();
    if (size.x > maxTsize.x || size.y > maxTsize.y || size.x == 0 || size.y == 0){
        return false;
    }
    bool success = m_atlas.Generate(size.x, size.y);
    return success && BeginRenderCall();*/
    return true;
}



bool SpriteAtlas::BeginRenderCall(){

    return true;
}
bool SpriteAtlas::RenderSprite(PointInt&& position,uint32_t spriteId){
    if (m_sprites.size() <= spriteId){
        return false;
    }

    return true;
}
bool SpriteAtlas::RenderCall(){


    return BeginRenderCall();
}
