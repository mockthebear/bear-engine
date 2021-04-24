#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS

#define VERT_POS_INDEX 0
#define COLOR_INDEX 1
#include "../glm/gtx/rotate_vector.hpp"
#include "../engine/genericstate.hpp"
#include "../engine/renderhelp.hpp"
#pragma once

class Test_Shapes: public State{
    public:
        Test_Shapes(){
            requestQuit = requestDelete = false;
            duration = 130.0f;
        };
        ~Test_Shapes(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test Shapes");
        }
        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){

	/*// Use the program object
	glUseProgram(programObject);

	glEnableVertexAttribArray(VERT_POS_INDEX);
	glBindBuffer(GL_ARRAY_BUFFER, gVboId);

	glVertexAttribPointer(VERT_POS_INDEX, 3, GL_FLOAT,
		GL_FALSE, 3 * sizeof(GLfloat), (const void*) 0);

	glEnableVertexAttribArray(COLOR_INDEX);
	glBindBuffer(GL_ARRAY_BUFFER, gVboColorId);

	glVertexAttribPointer(COLOR_INDEX, 4, GL_FLOAT,
		GL_FALSE, 4*sizeof(GLfloat), (const void*) 0);

	updateColor();

	glm::vec3 eye(0, 0, 2);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 target = eye + direction;
	gViewMatrix = glm::lookAt(eye, target, up);

	//gProjectionMatrix = glm::ortho(-1.0f, 1.0f, -0.75f, 0.75f, DefaultNearPlaneDistance, DefaultFarPlaneDistance);
	gProjectionMatrix = glm::perspective(30.0f, 1.0f / 0.75f, 1.0f, 20.0f);

	glm::mat4x4 mat;
	GLuint wmpLoc = glGetUniformLocation(programObject, "WorldViewProjection");
	updatePosition(mat);
	glm::mat4 wvp = gProjectionMatrix * gViewMatrix * mat;
	glUniformMatrix4fv(wmpLoc, 1, GL_FALSE, &wvp[0][0]);


	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);*/

	 int shid = Painter::textureShader.GetId();
	 Painter::textureShader.Bind();
        RenderHelp::DrawSquareColor(RectColor(Rect(32,32,32,38)));

    RenderHelp::DrawSquareColor(Rect(64,64,2,2),255,0,0,255);

    RenderHelp::DrawLineColor(LineColor(Point(32,32),g_input.GetMouse()));
            //RenderHelp::DrawSquareColor(Rect(64,64,2,2),255,0,0,255);

            RenderHelp::DrawSquareColor(Rect(32,32,64,64),255,255,100,255 *(1.0f - (duration/130.0)),false, 360.f * (duration/130.0) );
            RenderHelp::DrawSquareColor(Rect(32,32,64,64),255,255,255,255,true);
            RenderHelp::DrawSquareColor(Rect(64,64,2,2),255,0,0,255);


            RenderHelp::DrawLineColor(LineColor(96,96,128,128, BearColor(255,0,255,255)), 8.0);
            RenderHelp::DrawLineColor(LineColor(128,128,98,220, BearColor(255,0,0,255)));

            RenderHelp::DrawSquareColor(Rect(98,220,64,64),255,100,100,255,true);
            RenderHelp::DrawSquareColor(Rect(220,220,120,64),255,255,255,255,false);

            BearColor colors[4] = {
                BearColor(1.0f, 0.0f, 0.0f, 1.0f),
                BearColor(0.0f, 1.0f, 0.0f, 1.0f),
                BearColor(0.0f, 0.0f, 1.0f, 1.0f),
                BearColor(1.0f, 1.0f, 0.0f, 1.0f),
            };
            RenderHelp::DrawSquareColor(RectColor(Rect(320,320,120,64), colors));




            std::vector<LineColor> lines;
            std::vector<RectColor> squares;
            std::vector<RectColor> OutlineSquares;
            std::vector<PointColor> PointsRandom;
            std::vector<CircleColor> CircleRandom;

            Point OldP(0, 0);
            BearColor OldC(1.0f, 1.0f, 1.0f, 1.0f);


            CircleRandom.emplace_back( CircleColor( Circle(220,320,16), BearColor(1.0f, 1.0f, 1.0f, 0.0f), BearColor(1.0f, 1.0f, 1.0f, 1.0f) ) );
            CircleRandom.emplace_back( CircleColor( Circle(260,320,6), BearColor(0.0f, 1.0f, 1.0f, 1.0f), BearColor(1.0f, 0.0f, 1.0f, 1.0f) ) );
            CircleRandom.emplace_back( CircleColor( Circle(260,360,26), BearColor(0.0f, 1.0f, 0.0f, 1.0f), BearColor(1.0f, 0.0f, 0.0f, 0.0f) ) );




            for (int i=0;i<20;i++){
                float radAngle = Geometry::toRad( (360.0f / 20.0f) * (float)(i+duration* 0.2f) * 3.0f  );




                float baseCos = cos(radAngle);
                float baseSin = sin(radAngle);

                float range = (rand()%2000)/80.0f;

                float colr = (20/255.0f * (rand()%20));
                float colg = (20/255.0f * i);
                float colb = (20/255.0f * (rand()%20));
                PointsRandom.emplace_back(PointColor( Point(120 + baseCos *range,180 + baseSin *range ) , BearColor(colr,colg,colb,1.0f) ));


                float rColor = fabs(sin( radAngle ));
                float gColor = fabs(baseCos + baseSin);
                float bColor = fabs(baseCos);
                float x = 64 + i * 32;
                float y = 128 + 32*baseSin;
                if (i == 0){
                    OldP.x = x;
                    OldP.y = y;
                    OldC.r = rColor;
                    OldC.g = gColor;
                    OldC.b = bColor;
                }
                lines.emplace_back(LineColor(OldP,Point(x, y), OldC, BearColor(rColor, gColor, bColor, 1.0f)));


                OutlineSquares.emplace_back(Rect(x-16,y-16,32,32));

                RectColor hecc( Rect(x-16,y-16,32,32), BearColor(0.15,0.15,0.6,0.5) );
                hecc.colors[0] = BearColor(0.5f, 1.0f, 1.0f, 1.0f);

                hecc.colors[2] = BearColor(0.0f, 0.5f, 0.0f, 1.0f);
                squares.emplace_back(hecc);

                OldP.x = x;
                OldP.y = y;
                OldC.r = rColor;
                OldC.g = gColor;
                OldC.b = bColor;
            }



            RenderHelp::DrawSquaresColor(squares);
            RenderHelp::DrawSquaresColor(OutlineSquares, true);
            RenderHelp::DrawLinesColor(lines,2);


            RenderHelp::DrawPointsColor(PointsRandom);
            RenderHelp::DrawCirclesColor(CircleRandom);


            RenderHelp::DrawCircleColor(CircleColor(98,220,32,100,100,255,100) );
        };


        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
};

#endif // DISABLE_UNIT_TESTS
