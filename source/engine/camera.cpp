#include "camera.hpp"
#include "../input/inputmanager.hpp"
#include "../settings/configmanager.hpp"

float       Camera::speed = 12;
Rect        Camera::pos(0,0,0,0);
Rect        Camera::EffectArea(0,0,0,0);
Rect        Camera::UpdateArea(0,0,0,0);
GameObject* Camera::focus = NULL;

int         Camera::Offset_x;
int         Camera::Offset_y;
int         Camera::OffsetUpdate;
int         Camera::OffsetEffect;
bool        Camera::UseLimits=false;
float       Camera::maxX=0;
float       Camera::maxY=0;
float       Camera::minX=0;
float       Camera::minY=0;
float       Camera::MouseRange=32;

void Camera::LockLimits(float mx,float my,float x,float y){
    UseLimits = true;
    maxX = x;
    maxY = y;
    minX = mx;
    minY = my;
}

PointInt Camera::AdjustPosition(Point p){
    return PointInt(p.x-pos.x,p.y-pos.y);
}
PointInt Camera::AdjustPosition(Rect p){
    return PointInt(p.x-pos.x,p.y-pos.y);
}
PointInt Camera::AdjustPosition(Circle p){
    return PointInt(p.x-pos.x,p.y-pos.y);
}

PointInt Camera::AdjustPosition(Point p,float xof,float yof){
    return PointInt(p.x-pos.x+xof,p.y-pos.y+yof);
}
PointInt Camera::AdjustPosition(Rect p,float xof,float yof){
    return PointInt(p.x-pos.x+xof,p.y-pos.y+yof);
}
PointInt Camera::AdjustPosition(Circle p,float xof,float yof){
    return PointInt(p.x-pos.x+xof,p.y-pos.y+yof);
}

void Camera::Follow(GameObject *ob){
    if (ob != NULL){
        focus = ob;
        pos.x = floor(focus->box.x+focus->box.w/2.0-pos.w/2.0)+Offset_x;
        pos.y = floor(focus->box.y+focus->box.h/2.0-pos.h/2.0)+Offset_y;
        EffectArea.x = pos.x-OffsetEffect;
        EffectArea.y = pos.y-OffsetEffect;
        UpdateArea.x = pos.x-OffsetUpdate;
        UpdateArea.y = pos.y-OffsetUpdate;
    }else{
        Unfollow();
    }
}

void Camera::Resize(Point newDimension){
    pos.w = newDimension.x;
    pos.h = newDimension.y;
    EffectArea.w = newDimension.x;
    EffectArea.h = newDimension.y;
    EffectArea.w = newDimension.x;
    EffectArea.h = newDimension.y;

    EffectArea.w += OffsetEffect*2.0f;
    EffectArea.h += OffsetEffect*2.0f;
    UpdateArea.w += OffsetEffect*2.0f;
    UpdateArea.h += OffsetEffect*2.0f;

}
void Camera::Initiate(Rect startingPos,int offsetEffect_, int offsetUpdate_){
    OffsetEffect = offsetEffect_;
    OffsetUpdate = offsetUpdate_;
    pos = startingPos;
    EffectArea = startingPos;
    UpdateArea = startingPos;
    EffectArea.w += offsetEffect_*2.0f;
    EffectArea.h += offsetEffect_*2.0f;
    UpdateArea.w += offsetUpdate_*2.0f;
    UpdateArea.h += offsetUpdate_*2.0f;
}



void Camera::Update(float dt){
    if (focus != NULL){
        //focus = Penguins::player;
        //int x = focus->getX();e
        pos.x = floor(focus->box.x+focus->box.w/2.0-pos.w/2.0);
        pos.y = floor(focus->box.y+focus->box.h/2.0-pos.h/2.0);
        if (UseLimits){
            pos.x = std::max(minX,pos.x);
            pos.y = std::max(minY,pos.y);
            pos.y = std::min(maxY,pos.y+pos.h)-pos.h;
            pos.x = std::min(maxX,pos.x+pos.w)-pos.w;
        }
        pos.x += Offset_x;
        pos.y += Offset_y;
        EffectArea.x = pos.x-OffsetEffect/2.0;
        EffectArea.y = pos.y-OffsetEffect/2.0;
        UpdateArea.x = pos.x-OffsetUpdate/2.0;
        UpdateArea.y = pos.y-OffsetUpdate/2.0;
    }else{
        float sx,sy;
        Point mouse = InputManager::GetInstance().GetMouse();
        sx = sy = 0;
        if (InputManager::GetInstance().IsKeyDown(UP_ARROW_KEY) or mouse.y <= MouseRange){
            sy = sy-speed;
        }
        if (InputManager::GetInstance().IsKeyDown(DOWN_ARROW_KEY) or mouse.y >= ConfigManager::GetInstance().GetScreenH()-MouseRange){
            sy = sy+speed;
        }
        if (InputManager::GetInstance().IsKeyDown(LEFT_ARROW_KEY) or mouse.x <= MouseRange){
            sx = sx-speed;
        }
        if (InputManager::GetInstance().IsKeyDown(RIGHT_ARROW_KEY) or mouse.x >= ConfigManager::GetInstance().GetScreenW()-MouseRange){
            sx = sx+speed;
        }
        pos.y += sy*dt;
        pos.x += sx*dt;
        if (UseLimits){
            pos.x = std::max(minX,pos.x);
            pos.y = std::max(minY,pos.y);
            pos.y = std::min(maxY,pos.y+pos.h)-pos.h;
            pos.x = std::min(maxX,pos.x+pos.w)-pos.w;
        }
        EffectArea.x = pos.x-OffsetEffect/2.0;
        EffectArea.y = pos.y-OffsetEffect/2.0;
        UpdateArea.x = pos.x-OffsetUpdate/2.0;
        UpdateArea.y = pos.y-OffsetUpdate/2.0;

    }
}

