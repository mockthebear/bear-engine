#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../input/joystick.hpp"
#include "../input/inputmanager.hpp"
#pragma once

class Test_Joystick: public State{
    public:
        Test_Joystick(){
            requestQuit = requestDelete = false;

        };
        ~Test_Joystick(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Joystick or gamepad");
            bear::out << "Test joystick begin:\n";
            duration = 600.0f;
            bear::out << g_input.GetJoystickCount() << " joysticks detected.\n";
            if (g_input.GetJoystickCount() == 0){
                duration = 0.0f;
            }
            j1 = g_input.GetJoystick(0);
            if (j1){
                bear::out << "Info:\n" << j1->GetName() << "\n";
                bear::out << j1->GetAxisCount() << " axis\n";
                bear::out <<j1->GetBallCount()<<" balls\n";
                bear::out <<j1->GetButtonCount()<<" buttons\n";
                bear::out << j1->GetHatsCount() << " hats\n";
                bear::out << j1->GetInstanceId() << " instance id\n";
                bear::out <<"has haptics: " << j1->HasHaptics()<<"\n";
            }
        };

        void Update(float dt){
            duration -= dt;
            j1 = g_input.GetJoystick(0);
            if (!j1){
                bear::out << "No joystick found\n";
            }
            if( duration <= 0 ) {
                requestDelete = true;
            }



        };
        void Render(){
            j1 = g_input.GetJoystick(0);
            if (j1){
                for (int i=0;i<j1->GetButtonCount();i++){
                    InputState s = j1->GetButtonState(i);
                    int color = 0;
                    int color2 = 0;
                    if (s == JUST_PRESSED){
                        color = 255;
                    }else if(s == PRESSED){
                        color2 = 255;
                    }else if( s == JUST_RELEASED){
                        color = 127;
                    }else if( s == RELEASED){
                        color2 = 127;
                    }
                    RenderHelp::DrawSquareColorA(32 * i,32,28,28,255,color,color2,100);
                }

                for (int i=0;i<j1->GetHatsCount();i++){
                    for (int b=0;b<j1->GetHatButtonCount();b++){
                        InputState s = j1->GetHatState(i,b);
                        int color = 0;
                        int color2 = 0;
                        if (s == JUST_PRESSED){
                            color = 255;
                        }else if(s == PRESSED){
                            color2 = 255;
                        }else if( s == JUST_RELEASED){
                            color = 127;
                        }else if( s == RELEASED){
                            color2 = 127;
                        }
                        RenderHelp::DrawSquareColorA(32 * b,64 + 32*i,28,28,255,color,color2,100);
                    }
                }

                for (int i=0;i<j1->GetAxisCount();i++){

                    float s = j1->GetAxis(i);

                    float fSize = 330.0f;

                    bool preepr = abs(s) > (32767/2) ? true : false;

                    s = s / 32767.0f;
                    s = s * fSize;

                    if (s > 0){
                        RenderHelp::DrawSquareColorA(400,96 + 32*i,s,28,0,255,0,preepr ? 255 : 100);
                    }else{
                        s = - s;
                        RenderHelp::DrawSquareColorA(400 - s ,96 + 32*i,s,28,255,0,0,preepr ? 255 : 100);
                    }
                    RenderHelp::DrawSquareColorA(400 - fSize,96 + 32*i,fSize * 2,28,0,255,255,255,true);
                    InputState sE = j1->GetAxisInputState(i*2 +1);
                    RenderHelp::DrawSquareColorA(400 -fSize- 28 ,96 + 32*i,28,28,255,255,255,255,!(sE == PRESSED || sE == JUST_PRESSED));
                    sE = j1->GetAxisInputState(i*2 );
                    RenderHelp::DrawSquareColorA(400 +fSize ,96 + 32*i,28,28,255,255,255,255,!(sE == PRESSED || sE == JUST_PRESSED));



                }

            }
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Joystick *j1;
        float duration;
};


