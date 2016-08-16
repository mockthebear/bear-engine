#pragma once


#include <unordered_map>
#include <string>
#include <functional>
#include "../settings/definitions.hpp"
#include "../framework/geometry.hpp"
#include "inputdefinitions.hpp"
#include SDL_LIB_HEADER
/**
 * @brief This class manages the joystick
 *
 * First you need to get the joystick. Check InputManager::GetJoystick
 @code
 #include "../input/inputmanager.hpp"
 void Somethin::Update(float dt){
    //bla bla bla
    Joystick *device = InputManager::GetInstance().GetJoystick(0);
    if (device){
        //ALWAYS CHECK IF THE DEVICE IS NOT NULL
        if (device->ButtonPress(1)){
           x += 2.0*dt;
        }
    }else{
        //Joystick 1 is disconnected
    }
 }
 @endcode
 * Consider checking the enum InputState.

 */

class Joystick{
    public:
        /**
            *Dont waste your time in this.
            *Its fully automatic
        */
        Joystick();
        /**
            *Dont waste your time in this.
            *Its fully automatic
        */
        Joystick(int index);
        /**
            *Dont waste your time in this.
            *Its fully automatic
        */
        ~Joystick();
        /**
            *Dont waste your time in this.
            *Its fully automatic
        */
        void Close();
        void Update(float dt);

        /**
            *May the SDL2 or Bear Engine mess with some device things.
            *This can check if the joystick is working
            @return true or false
        */
        inline bool IsWorking(){return m_joyHandler != NULL;};
        /**
            *Work the same way as Joystick::AddButtonCallBack, Joystick::AddAxisCallBack, Joystick::AddBallCallBack and Joystick::AddHatCallBack;
            *Check Joystick::AddButtonCallBack to see how to use the callback
            @param CallbackF Lambda function void(int joyId,int Button,InputState Value)> CallbackF
            @code
            Joystick *device = InputManager::GetInstance().GetJoystick(0);
            if (device){
                device->AddButtonCallBack([=](int joyId,int Button,InputState Value){
                    Console::GetInstance().AddText(utils::format("Joystick %d changed the state of the key %d",joyId,Button));
                });
            }
            @endcode
        */
        void AddButtonCallBack(std::function<void(int joyId,int button,InputState Value)> CallbackF);
        /**
            *Remove the callback
        */
        void ClearButtonCallBack();
        /**
            *Work the same way as Joystick::AddButtonCallBack, Joystick::AddAxisCallBack, Joystick::AddBallCallBack and Joystick::AddHatCallBack;
            *Check Joystick::AddButtonCallBack to see how to use the callback
            @param CallbackF Lambda function void(int joyId,int axisId,int Value)> CallbackF
        */
        void AddAxisCallBack(std::function<void(int joyId,int axisId,int Value)> CallbackF);
        /**
            *Remove the callback
        */
        void ClearAxisCallBack();
        /**
            *Work the same way as Joystick::AddButtonCallBack, Joystick::AddAxisCallBack, Joystick::AddBallCallBack and Joystick::AddHatCallBack;
            *Check Joystick::AddButtonCallBack to see how to use the callback
            @param CallbackF Lambda function void(int joyId,int ballId,int ValueX,int ValueY)
        */
        void AddBallCallBack(std::function<void(int joyId,int ballId,int ValueX,int ValueY)> CallbackF);
        /**
            *Remove the callback
        */
        void ClearBallCallBack();
        /**
            *Work the same way as Joystick::AddButtonCallBack, Joystick::AddAxisCallBack, Joystick::AddBallCallBack and Joystick::AddHatCallBack;
            *Check Joystick::AddButtonCallBack to see how to use the callback
            @param CallbackF Lambda function void(int joyId,int hatId,int Value)
        */
        void AddHatCallBack(std::function<void(int joyId,int hatId,int Value)> CallbackF);
        /**
            *Remove the callback
        */
        void ClearHatCallBack();

        /**
            *Remove ALL the callbacks
        */
        void ClearAllCallBacks();

        /**
            *Used by InputManager.
        */
        void Button(int button,int state);
        /**
            *Used by InputManager.
        */
        void MoveAxis(int axisId,int value);
        /**
            *Used by InputManager.
        */
        void MoveHat(int hatId,int value);
        /**
            *Used by InputManager.
        */
        void MoveBall(int ballId,int vx,int vy);

        /**
            Return true at the moment of button press
            @param key 0..n Check Joystick::GetButtonCount
            @return true or false
        */
        bool ButtonPress(int key);
        /**
            Return true at the moment of button release
            @param key 0..n Check Joystick::GetButtonCount
            @return true or false
        */
        bool ButtonRelease(int key);
        /**
            Return true while the button is pressed
            @param key 0..n Check Joystick::GetButtonCount
            @return true or false
        */
        bool IsButtonDown(int key);
        /**
            Return true while the button is released
            @param key 0..n Check Joystick::GetButtonCount
            @return true or false
        */
        bool IsButtonUp(int key);
        /**
            *Use to debug joystick. Not called by the engine.
            *When you call this function, some data about the joystick will be shown on screen
            @param x position in x
            @param y position in y
        */
        void Render(int id);

        /**
            *Joysticks can have more than one axis. This function return the value from the given axis
            *Check Joystick::GetAxisCount
            @param axisId -32767 , +32767
            @return true or false
        */
        int GetAxis(int axisId){return Axis[axisId];};
        int *Axis;
        /**
            *Joysticks can have more than one ball. This function return the value from the given ball
            *Check Joystick::GetBallCount
            @param PointInt x and y
            @return true or false
        */
        PointInt GetBall(int ballId){return Balls[ballId];};

        /**
            Return the state of a given button
            @param button Button number Joystick::GetButtonCount
            @return true or false
        */
        InputState GetButtonState(int button){return Buttons[button];};
        /**
            Return the SDL2 device ID of this joystick
            @return int
        */
        int GetInstanceId(){return m_id;};

        /**
            Get the amount of axis in the current joystick
            @return int
        */
        int GetAxisCount(){return m_axes;};
        /**
            Get the amount of buttons in the current joystick
            @return int
        */
        int GetButtonCount(){return m_buttons;};
        /**
            Get the amount of balls in the current joystick
            @return int
        */
        int GetBallCount(){return m_balls;};
        /**
            Get the amount of hats in the current joystick
            @return int
        */
        int GetHatsCount(){return m_hats;};

        std::string GetName(){return m_name;};
    private:
        /* Buttons */
        std::unordered_map<int, InputState> Buttons;
        bool HasCallButton;
        std::function<void(int joyId,int Button,InputState Value)> ButtonCallBack;

        /* Axis */
        bool HasCallAxis;
        std::function<void(int joyId,int axisId,int Value)> AxisCallBack;


        /* Ball */
        bool HasCallBall;
        std::function<void(int joyId,int axisId,int ValueX,int ValueY)> BallCallBack;
        PointInt *Balls;
        /* Axis */
        bool HasCallHat;
        std::function<void(int joyId,int axisId,int Value)> HatCallBack;
        int *Hats;

        std::string m_name;
        int m_id;
        int m_axes,m_buttons,m_balls,m_hats;
        SDL_Joystick *m_joyHandler;
        SDL_Haptic *m_haptic;
        bool anyKeyPressed;



};

