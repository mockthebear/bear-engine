#include "camera.hpp"
#include "../input/inputmanager.hpp"
#include "../settings/configmanager.hpp"

float       Camera::speed = 12;
RectInt     Camera::pos(0,0,0,0);
RectInt     Camera::RealPos(0,0,0,0);
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
    return PointInt((int)(p.x-RealPos.x),(int)(p.y-RealPos.y));
}
PointInt Camera::AdjustPosition(Rect p){
    return PointInt(p.x-RealPos.x,p.y-RealPos.y);
}
PointInt Camera::AdjustPosition(Circle p){
    return PointInt(int(p.x-RealPos.x), int(p.y-RealPos.y));
}

PointInt Camera::AdjustPosition(Point p,float xof,float yof){
    return PointInt(p.x-RealPos.x+xof,p.y-RealPos.y+yof);
}
PointInt Camera::AdjustPosition(Rect p,float xof,float yof){
    return PointInt(p.x-RealPos.x+xof,p.y-RealPos.y+yof);
}
PointInt Camera::AdjustPosition(Circle p,float xof,float yof){
    return PointInt(p.x-RealPos.x+xof,p.y-RealPos.y+yof);
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
    RealPos.w = (int)newDimension.x;
    RealPos.h = (int)newDimension.y;
    EffectArea.w = (int)newDimension.x;
    EffectArea.h = (int)newDimension.y;
    EffectArea.w = (int)newDimension.x;
    EffectArea.h = (int)newDimension.y;

    EffectArea.w += (int)(OffsetEffect*2.0f);
    EffectArea.h += (int)(OffsetEffect*2.0f);
    UpdateArea.w += (int)(OffsetEffect*2.0f);
    UpdateArea.h += (int)(OffsetEffect*2.0f);
    pos = RealPos;

}
void Camera::Initiate(Rect startingPos,int offsetEffect_, int offsetUpdate_){
    OffsetEffect = offsetEffect_;
    OffsetUpdate = offsetUpdate_;
    pos = startingPos;
    RealPos = startingPos;
    EffectArea.x = startingPos.x;
    EffectArea.y = startingPos.y;
    UpdateArea.x = startingPos.y;
    UpdateArea.y = startingPos.y;
    EffectArea.w = RealPos.w+ offsetEffect_*2.0f;
    EffectArea.h = RealPos.h+ offsetEffect_*2.0f;
    UpdateArea.w = RealPos.w+ offsetUpdate_*2.0f;
    UpdateArea.h = RealPos.h+ offsetUpdate_*2.0f;
    focus = nullptr;
}


void Camera::UpdateByPos(Rect box,float dt){
    Point toGo;
    toGo.x = floor(box.getXCenter());
    toGo.y = floor(box.getYCenter());
    Point posCenter(RealPos.getXCenter(),RealPos.getYCenter());
    if (Smooth && toGo.getDistance(posCenter) >= speed*dt ){
        float angle = toGo.getDirection(posCenter);
        RealPos.x += (int)(cos(angle)*dt*speed);
        RealPos.y += (int)(sin(angle)*dt*speed);
    }else{
        RealPos.x = (int)(box.x+box.w/2-RealPos.w/2);
        RealPos.y = (int)(box.y+box.h/2-RealPos.h/2);
    }
    if (UseLimits){
        RealPos.x = std::max((int)minX,RealPos.x);
        RealPos.y = std::max((int)minY,RealPos.y);
        RealPos.y = std::min((int)maxY,RealPos.y);
        RealPos.x = std::min((int)maxX,RealPos.x);
    }
    EffectArea.x = RealPos.x-OffsetEffect;
    EffectArea.y = RealPos.y-OffsetEffect;
    UpdateArea.x = RealPos.x-OffsetUpdate;
    UpdateArea.y = RealPos.y-OffsetUpdate;
    pos = RealPos;
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

        EffectArea.x = (int)(RealPos.x-OffsetEffect/2.0f);
        EffectArea.y = (int)(RealPos.y-OffsetEffect/2.0f);
        UpdateArea.x = (int)(RealPos.x-OffsetUpdate/2.0f);
        UpdateArea.y = (int)(RealPos.y-OffsetUpdate/2.0f);

    }
}

