#ifndef INPUH
#define INPUH
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include "../framework/geometry.hpp"

#include <functional>
#include <map>
#include <queue>

#include "inputdefinitions.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"
#include "touch.hpp"
#include "joystick.hpp"
#include "accelerometer.hpp"
#include "touchkeys.hpp"

#define LEFT_ARROW_KEY SDLK_LEFT
#define RIGHT_ARROW_KEY SDLK_RIGHT
#define UP_ARROW_KEY SDLK_UP
#define DOWN_ARROW_KEY SDLK_DOWN
#define ESCAPE_KEY SDLK_ESCAPE
#define LEFT_MOUSE_BUTTON SDL_BUTTON_LEFT
/**
 * @brief An singleton class to manage 99.9% of the game inputs.
 *
 * This class is managed by the internal Game class.
 * Should be used like this:
 @code
 #include "../input/inputmanager.hpp"
 void Somethin::Update(float dt){
    Point pos = InputManager::GetInstance().GetMouse();
     //..
     //..
     if (InputManager::GetInstance().KeyPress(SDLK_a)){
        x += 2.0*dt;
     }
 }
 @endcode
 * Consider checking the enum InputState.
 * Most of functions work based on the enum to say if the current
 * key is just pressed (at a given moment), the it keeps pressed or
 * was just released.
 *
 * This class can manage Joystick, Mouse, Keyboard
 * Also Touch is avaliable in all builds but
 * may not be used in your game
 * The Accelerometer can be managed too, but only work on android.

 */

class InputManager{
    public:
        /**
            * Dont waste your time trying to update the input manager.
            * The engine already does it before updating your game state.
            @param dt Deleay between the last update
        */
        void Update(float dt);
        void Render();
        /**
            @brief Return true at the moment of press
            *
            @param key Use SDLK_[name]. Like SDLK_w
            @return true or false
        */
        bool KeyPress(int key);
        /**
            @brief Return true at the moment of release
            @param key Use SDLK_[name]. Like SDLK_w
            @return true or false
        */
        bool KeyRelease(int key);
        /**
            @brief Return if the key is pressed at any time
            @param key Use SDLK_[name]. Like SDLK_w
            @return true or false
        */
        bool IsKeyDown(int key);
        /**
            @brief Return if the key is released at any time
            @param key Use SDLK_[name]. Like SDLK_w
            @return true or false
        */
        bool IsKeyUp(int key);
        /**
            @brief Literally, true for any kay pressed
            @return the pressed SDLK id
        */
        int IsAnyKeyPressed(bool considerTap = false);
        int IsScreenTapped();
        /**
            @brief When requested by input to quit
            @return true or false
        */
        bool ShouldQuit (){return quitGame;};
        /**
            @brief Use this to get the singleton reference
            @code
            if (InputManager::GetInstance().IsKeyUp(SDLK_s)){
                //Do something
            }
            @endcode
            @return Reference to singleton
        */
        static InputManager& GetInstance();
        /**
            *This is creator
            *Created automatically, dont waste your time with this.
        */
        InputManager();
        /**
            *This is the destructor
            *Deleted automatically, dont waste your time with this.
        */
        ~InputManager();

        //Mouse
        /**
            *Return true at the moment of the mouse press.
            *<b>Not while</b>
            @param key SDL_BUTTON_LEFT
            @return true or false
        */
        bool MousePress(int key);
        /**
            *Return true at the moment of the mouse release.
            *<b>Not while</b>
            @param key SDL_BUTTON_LEFT
            @return true or false
        */
        bool MouseRelease(int key);
        /**
            Return true while the mouse is pressed.
            @param key SDL_BUTTON_LEFT
            @return true or false
        */
        bool IsMousePressed(int key);
        /**
            Return true while the mouse is released.
            @param key SDL_BUTTON_LEFT
            @return true or false
        */
        bool IsMouseReleased(int key){
            return not IsMousePressed(key);
        };

        /**

            Return true if the mouse is inside a Rect
            @param rect The rect (x,y,w,h)
            @return true or false
        */
        bool IsMouseInside (Rect rect);
        /**
            Get the current pressed mouse button.
            @return 0-N. N means the amount of buttons in your mouse.
        */
        int GetMouseMousePressKey();
        /**
            Get the current released mouse button.
            @return 0-N. N means the amount of buttons in your mouse.
        */
        int GetMouseMouseReleaseKey();
        /**
            Return the position of the wheel in X?
            @return int
        */
        inline int GetWheelX(){return mouse.wheel.x;};
        /**
            Return the position of the wheel in Y
            @return int
        */
        inline int GetWheelY(){return mouse.wheel.y;};
        /**
            Get the mouse position in X
            @return int
        */
        int GetMouseX();
        /**
            Get the mouse position in Y
            @return int
        */
        int GetMouseY();
        /**
            Get the mouse position as a Point
            @return Point
        */
        inline Point GetMouse(){return Point(GetMouseX(),GetMouseY());};

        /**
            *This can be used of calibration
            *When you see your touchscreen with a offset from where are your finger.
            *Set this to fix.
            @param offset Means the offset as a Point to x,y
        */
        void SetTouchOffset(Point offset){touchscreen.SetOffset(offset);};
        /**
            *Return true if the finger is pressed at the given moment
            *<b>Not while</b>
            @param i The finger. Depending from your device can be up to 10
            @return true or false
        */
        bool FingerPress(int fingerId){return touchscreen.fingers[fingerId].m_state == JUST_PRESSED;};
        /**
            *Return true if the finger is pressed
            @param fingerId The finger. Dpending from your device can be up to 10
            @return true or false
        */
        bool IsFingerPressed(int fingerId){return touchscreen.fingers[fingerId].m_state == JUST_PRESSED or touchscreen.fingers[fingerId].m_state == PRESSED;};
        /**
            *Get the Point position from an given finger.
            @param fingerId When set to no argument or -1, the return position will be from the last finger that touched the screen
            @return true or false
        */
        PointInt GetFingerPosition(int fingerId=-1){return touchscreen.GetFingerPosition(fingerId);};
        /**
            *Use to get the information about an finger
            @param fingerId When set to no argument or -1, the return position will be from the last finger that touched the screen
            @return TouchInfo
        */
        TouchInfo GetFinger(int fingerId=-1){return touchscreen.GetFinger(fingerId);};
        /**
            *Get the true position from X get right from SDL2
            @param fingerId When set to no argument or -1, the return position will be from the last finger that touched the screen
            @return float [0..1]
        */
        float GetTouchTrueX(int fingerId){return touchscreen.fingers[fingerId].m_truePosition.x;};
        /**
            *Get the true position from Y get right from SDL2
            @param i When set to no argument or -1, the return position will be from the last finger that touched the screen
            @return float [0..1]
        */
        float GetTouchTrueY(int fingerId){return touchscreen.fingers[fingerId].m_truePosition.y;};

        /**
            *Get the amount of joystick connected to the device
            @return int 0...N
        */
        int GetJoystickCount(){return NumJoysticks + (HasAccelerometer ? -1 : 0);};
        /**
            *Get the joystick.
            *This is not the ID from the device nor the SDL2 id. Its an internal id to say
            *"Hey, this is the joystick 0, so it will be always the first joystick, okay?"
            *So, when you have two joysticks connected, then you remove the first one
            *the return to id=0 will be always NULL,
            *When the joystick reconnect, the id=0 will return the new device.

            @param i The joystick 0..1..2...
            @return Pointer to a Joystick class
        */
        Joystick* GetJoystick(int id);

        /**
            *Wip
        */
        void Vibrate(uint32_t milis);

        void CreateVirtualButton(int Key,PointInt size,PointInt position,std::string text);

        /**

        */
        bool HasAccelerometerEnabled(){return HasAccelerometer;};
        float GetAccelerometerX(){return m_acceletometer.GetXPercent();};
        float GetAccelerometerY(){return m_acceletometer.GetXPercent();};
        float GetAccelerometerZ(){return m_acceletometer.GetXPercent();};
        Point3 GetAccelerometerForces(){return m_acceletometer.GetPercent();};
    private:
        bool HasAccelerometer;
        int AccelerometerJoystick;
        Accelerometer m_acceletometer;
        int NumJoysticks;
        std::vector<TouchKeys> TKeys;
        std::unordered_map<int,Joystick*> Joysticks;
        int JoyPos[8];
        /*
            This value will be used to say "this is joystick 1, and this is 2...
            when disconected, set to -1
            when it reconencts, set to default value back.
        */
        Mouse mouse;
        Keyboard keyboard;
        Touch touchscreen;
        bool quitGame;

};
#endif
