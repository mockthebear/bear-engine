#include "../framework/geometry.hpp"
#include "object.hpp"
#include "../settings/definitions.hpp"
#include <stdio.h>
#ifndef CAMERAH
#define CAMERAH
/**
    @brief Camera. One of the most important classed in the engine
    *
    * This class is important because it will rule what and where something will be rendered.\n
    * Also tells what objects can be rendered or updated.

*/
class Camera{
    public:
        /**
            The true camera position
        */
        static RectInt pos;
        /**
            An area bigger than Camera::pos, it will tell what instances can be rendered
        */
        static RectInt EffectArea;
        /**
            An area bigger than Camera::pos and maybe same size as Camera::EffectArea, it will tell what instances can be updated
        */
        static RectInt UpdateArea;
        /**
            *This will startup the camera with the default values of the screen as the main Rect\n
            *Start a 0,0
            *The pixels amount of the Camera::EffectArea bigger than Camera::pos will be 32
            *The pixels amount of the Camera::UpdateArea bigger than Camera::pos will be 32
        */
        static void Initiate(Rect startingPos=Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),int offsetEffect=32, int OffsetUpdate=32);
        /**
            Resize the camera to an new rect. Keep the differences sizes of the Camera::EffectArea and Camera::UpdateArea
        */
        static void Resize(Point newDimension);
        /**
            Used to move the camera only in its position
        */
        void operator=(const RectInt& b){ pos = b;};
        /**
            Used to move the camera only in its position
        */
        void operator=(const Point& b){ pos.x = b.x; pos.y=b.y;};
        /**
            Camera speed
        */
        static float speed;
        /**
            Set the camera to follow one object
        */
        static void Follow(GameObject *ob,bool smooth = false);
        /**
            Disabled following
        */
        static void Unfollow(){focus = NULL;};
        /**
            Set an offset to following thing
        */
        static void SetOffsetFollow(int x,int y){Offset_x=x,Offset_y=y;};
        /**
            *You must call this function
        */
        static void Update(float dt);
        /**
            Disable camera limitations on Camera::LockLimits
        */
        static void UnlockLimits(){UseLimits=false;};
        /**
            Create an rect with an limit that the camera can follow
        */
        static void LockLimits(float x,float y,float mx,float my);

        /**
            Ajust an position or an Rect to get the rendering position
            @code
                objSprite.Render(Camera::AdjustPosition(box));
            @endcode
        */
        static PointInt AdjustPosition(Point p,float xof,float yof);
        /**
            Ajust an position or an Rect to get the rendering position
            @code
                objSprite.Render(Camera::AdjustPosition(pointPos));
            @endcode
        */
        static PointInt AdjustPosition(Rect p,float xof,float yof);
        /**
            Ajust an position or an Rect to get the rendering position
            @code
                objSprite.Render(Camera::AdjustPosition(circle));
            @endcode
        */
        static PointInt AdjustPosition(Circle p,float xof,float yof);

        static PointInt AdjustPosition(Point p);
        static PointInt AdjustPosition(Rect p);
        static PointInt AdjustPosition(Circle p);
        static float maxX,maxY,minX,minY,MouseRange;
        static bool UseLimits;
    private:
    static GameObject *focus;
    static int Offset_x,Offset_y;

    static bool Smooth;
    static int OffsetEffect,OffsetUpdate;

};

#endif
