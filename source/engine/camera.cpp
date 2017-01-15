#include "camera.hpp"
#include "../input/inputmanager.hpp"
#include "../settings/configmanager.hpp"

float       Camera::speed = 12;
RectInt     Camera::pos(0,0,0,0);
RectInt     Camera::EffectArea(0,0,0,0);
RectInt     Camera::UpdateArea(0,0,0,0);
GameObject* Camera::focus = nullptr;
Point*      Camera::focusPoint = nullptr;
Rect*       Camera::focusRect = nullptr;

int         Camera::Offset_x = 0;
int         Camera::Offset_y = 0;
int         Camera::OffsetUpdate;
int         Camera::OffsetEffect;
bool        Camera::UseLimits=false;
bool        Camera::Smooth=false;
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
    return PointInt((int)(p.x-pos.x),(int)(p.y-pos.y));
}
PointInt Camera::AdjustPosition(Rect p){
    return PointInt(p.x-pos.x,p.y-pos.y);
}
PointInt Camera::AdjustPosition(Circle p){
    return PointInt(int(p.x-pos.x), int(p.y-pos.y));
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

void Camera::Follow(GameObject *ob,bool smoothed){
    Unfollow();
    if (ob != NULL){
        Smooth = smoothed;
        focus = ob;
    }
}

void Camera::Follow(Rect *ob,bool smoothed){
    Unfollow();
    if (ob != NULL){
        Smooth = smoothed;
        focusRect = ob;
    }
}

void Camera::Resize(Point newDimension){
    pos.w = (int)newDimension.x;
    pos.h = (int)newDimension.y;
    EffectArea.w = (int)newDimension.x;
    EffectArea.h = (int)newDimension.y;
    EffectArea.w = (int)newDimension.x;
    EffectArea.h = (int)newDimension.y;

    EffectArea.w += (int)(OffsetEffect*2.0f);
    EffectArea.h += (int)(OffsetEffect*2.0f);
    UpdateArea.w += (int)(OffsetEffect*2.0f);
    UpdateArea.h += (int)(OffsetEffect*2.0f);

}
void Camera::Initiate(Rect startingPos,int offsetEffect_, int offsetUpdate_){
    OffsetEffect = offsetEffect_;
    OffsetUpdate = offsetUpdate_;
    pos = startingPos;
    EffectArea.x = startingPos.x;
    EffectArea.y = startingPos.y;
    UpdateArea.x = startingPos.y;
    UpdateArea.y = startingPos.y;
    EffectArea.w = pos.w+ offsetEffect_*2.0f;
    EffectArea.h = pos.h+ offsetEffect_*2.0f;
    UpdateArea.w = pos.w+ offsetUpdate_*2.0f;
    UpdateArea.h = pos.h+ offsetUpdate_*2.0f;
    focus = nullptr;
}


void Camera::UpdateByPos(Rect box,float dt){
    Point toGo;
    toGo.x = floor(box.getXCenter());
    toGo.y = floor(box.getYCenter());
    Point posCenter(pos.getXCenter(),pos.getYCenter());
    if (Smooth && toGo.getDistance(posCenter) >= speed*dt ){
        float angle = toGo.getDirection(posCenter);
        pos.x += (int)(cos(angle)*dt*speed);
        pos.y += (int)(sin(angle)*dt*speed);
    }else{
        pos.x = (int)(box.x+box.w/2-pos.w/2);
        pos.y = (int)(box.y+box.h/2-pos.h/2);
    }
    if (UseLimits){
        pos.x = std::max((int)minX,pos.x);
        pos.y = std::max((int)minY,pos.y);
        pos.y = std::min((int)maxY,pos.y);
        pos.x = std::min((int)maxX,pos.x);
    }
    EffectArea.x = pos.x-OffsetEffect;
    EffectArea.y = pos.y-OffsetEffect;
    UpdateArea.x = pos.x-OffsetUpdate;
    UpdateArea.y = pos.y-OffsetUpdate;

    pos.x += Offset_x;
    pos.y += Offset_y;
}
void Camera::Update(float dt){
    if (focus != NULL){
        UpdateByPos(focus->box,dt);
    }else if (focusPoint != NULL){
        UpdateByPos(Rect(focusPoint->x,focusPoint->y,1,1),dt);
    }else if (focusRect != NULL){
        UpdateByPos(*focusRect,dt);
    }else{
        /*float sx,sy;
        Point mouse = InputManager::GetInstance().GetMouse();
        sx = sy = 0;
        if (InputManager::GetInstance().IsKeyDown(UP_ARROW_KEY) || mouse.y <= MouseRange){
            sy = sy-speed;
        }
        if (InputManager::GetInstance().IsKeyDown(DOWN_ARROW_KEY) || mouse.y >= ConfigManager::GetInstance().GetScreenH()-MouseRange){
            sy = sy+speed;
        }
        if (InputManager::GetInstance().IsKeyDown(LEFT_ARROW_KEY) || mouse.x <= MouseRange){
            sx = sx-speed;
        }
        if (InputManager::GetInstance().IsKeyDown(RIGHT_ARROW_KEY) || mouse.x >= ConfigManager::GetInstance().GetScreenW()-MouseRange){
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
        */

        EffectArea.x = (int)(pos.x-OffsetEffect/2.0f);
        EffectArea.y = (int)(pos.y-OffsetEffect/2.0f);
        UpdateArea.x = (int)(pos.x-OffsetUpdate/2.0f);
        UpdateArea.y = (int)(pos.y-OffsetUpdate/2.0f);

    }
}

