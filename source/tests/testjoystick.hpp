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
            if (j1){


            }
            //if( InputManager::GetInstance().IsAnyKeyPressed() || duration <= 0 ) {
            //    requestDelete = true;
            //}



        };
        void Render(){
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

            }
        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Joystick *j1;
        float duration;
};


