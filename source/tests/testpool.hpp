#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



#include "../engine/genericstate.hpp"
#include "testobject.hpp"
#pragma once

class Test_Pool: public State{
    public:
        Test_Pool(){

            requestQuit = requestDelete = false;
            duration = 200.0f;
            objAdder = 10.0f;
        };
        ~Test_Pool(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test pool");
            /*
            If the first parameter was false. The object 2 would not be added.
            This object 2 will be added to an list, not to a pool. Basically an std::vector<GameObject*>
            */

            /*
                Pre alloc 10 test objects.
            */
            Pool.Register<TestObject>(10);

            Pool.AddInstance(TestObject2(Point(90,90)));





            Pool.AddInstance(TestObject(Point(32,32)));
            TestObject obj = TestObject({55,55});
            Pool.AddInstance(obj);

            Camera::Initiate();

        };

        void Update(float dt){
            UpdateInstances(dt);
            //duration -= dt;
            objAdder -= dt;
            if (objAdder <= 0){
                Pool.AddInstance(TestObject(Point(90,90)));
                objAdder = 10.0f;
            }
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){
            RenderInstances();
        };
        void Input();
        void Resume(){};
        void End(){
            Pool.ErasePools();
        };
    private:
        float objAdder;
        float duration;
};

#endif // RUN_TESTS
