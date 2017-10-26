#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/text.hpp"
#pragma once

class Test_Text: public State{
    public:
        Test_Text(){
            requestQuit = requestDelete = false;
            duration = 100.0f;
            angle = 0;
        };
        ~Test_Text(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Text");

            title = Text("Basic text",34,{255,255,255});
            underline = Text("hmmmmm",28,{100,100,100});
            rotating = Text("ui/UnB-Office_Regular.ttf",45,TEXT_SOLID,"Rotating",{255,33,180});
            rotating.SetAliasign(TEXTURE_TRILINEAR);
            randomText = Text("???",28,{140,255,100});
            whoa = Text("whoa",28,{140,255,100});
            copied = randomText;
            aliasing = Text("ALIASING",22,{255,255,180});
            noaliasing = Text("NOT ALIASING",22,{255,255,180});
            aliasing.SetAliasign(TEXTURE_TRILINEAR);
        };

        void Update(float dt){
            duration -= dt;
            angle += 0.5*dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }
            whoa.SetColor({ (uint8_t)(rand()%255),(uint8_t)(rand()%255),(uint8_t)(rand()%255) });
            rotating.SetRotation( Geometry::toDeg(angle) );

            randomText.SetText( utils::format("%c%c%c", 'a' + rand()%int('z'-'a'),'a' + rand()%int('z'-'a'),'a' + rand()%int('z'-'a') ) );

        };
        void Render(){

            RenderHelp::DrawSquareColor(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H,100,100,100,255);

            Text localText;
            title.Render(Point(0,0),TEXT_RENDER_TOPLEFT);
            underline.Render(Point(0,title.GetHeight()));
            rotating.Render(Point(SCREEN_SIZE_W/2,SCREEN_SIZE_H/2),TEXT_RENDER_CENTER);
            randomText.Render(Point(SCREEN_SIZE_W/2,120));

            copied.Render(Point(SCREEN_SIZE_W/2,520));
            RenderHelp::DrawSquareColorA(SCREEN_SIZE_W/2,520,copied.GetWidth(),copied.GetHeight(),255,255,255,255,true);

            localText = copied;
            RenderHelp::DrawSquareColorA(SCREEN_SIZE_W/2 + 86,520,localText.GetWidth(),localText.GetHeight(),255,255,255,255,true);
            localText.Render(Point(SCREEN_SIZE_W/2 + 86,520));

            whoa.Render(Point(32,520));


            aliasing.Render(Point(400,320),TEXT_RENDER_TOPRIGHT);
            noaliasing.Render(Point(400,320 +aliasing.GetHeight() ),TEXT_RENDER_TOPRIGHT);

            Console::GetInstance().Render();

        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        Text title;
        Text underline;

        Text rotating;
        Text randomText;
        Text copied;
        Text whoa;
        Text aliasing;
        Text noaliasing;
        float angle;

        float duration;
};


