#pragma once
#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS


#include "../engine/gamebase.hpp"

#include "testatlas.hpp"
#include "testfiles.hpp"
#include "testsprite.hpp"
#include "testlua.hpp"
#include "testpathfind.hpp"
#include "testpool.hpp"
#include "testjoystick.hpp"
#include "testtext.hpp"
#include "testshapes.hpp"
#include "testtargettexture.hpp"
#include "testshader.hpp"
#include "testsocket.hpp"
#include "testsound.hpp"
#include "testrenderspeed.hpp"
#include "testpostprocess.hpp"
#include "testthreads.hpp"
#include "testfasttile.hpp"
#include "testrectrenderspeed.hpp"
#include "testnode.hpp"
#include "testnodetileset.hpp"
#include "testnewlight.hpp"
#include "testblank.hpp"
#include "testnoise.hpp"
#endif // DISABLE_UNIT_TESTS

void SetupTests();


void SetupTests(){

    #ifndef DISABLE_UNIT_TESTS





    //#ifdef THREADPOOLTEST
    //Game::GetInstance()->AddState(new Test_Threadpool());
    //#endif // THREADPOOLTEST


    #ifdef LUATESTSUITE
    Game::GetInstance()->AddState(new Test_Lua());
    #endif // LUATESTSUITE

    /*Game::GetInstance()->AddState(new Test_Sound());

    Game::GetInstance()->AddState(new Test_Joystick());

    Game::GetInstance()->AddState(new Test_NewLight());

    Game::GetInstance()->AddState(new Test_PostProcess());

    Game::GetInstance()->AddState(new Test_RenderSpeed());
    Game::GetInstance()->AddState(new Test_RectRenderSpeed());
    Game::GetInstance()->AddState(new Test_PathFind());
    Game::GetInstance()->AddState(new Test_Text());
    Game::GetInstance()->AddState(new Test_NodeTileset());
    Game::GetInstance()->AddState(new Test_FastTiles());
    Game::GetInstance()->AddState(new Test_Thread());
    Game::GetInstance()->AddState(new Test_Atlas());
    Game::GetInstance()->AddState(new Test_Perlin());
    Game::GetInstance()->AddState(new Test_TargetTexture());


    Game::GetInstance()->AddState(new Test_Node());
    Game::GetInstance()->AddState(new Test_Pool());
    */

    Game::GetInstance()->AddState(new Test_Sound());

    Game::GetInstance()->AddState(new Test_Perlin());
    Game::GetInstance()->AddState(new Test_TargetTexture());
    Game::GetInstance()->AddState(new Test_Shader());

    Game::GetInstance()->AddState(new Test_Sprite());
    Game::GetInstance()->AddState(new Test_Shapes());
    Game::GetInstance()->AddState(new Test_Files());

    #endif // DISABLE_UNIT_TESTS
}

