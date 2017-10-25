/*
    This is not a state. Just an object.
*/
#include "../engine/object.hpp"
#include "../engine/renderhelp.hpp"


class TestObject: public GameObject{
    public:
    /*
        Necessary an empty constructor IF you using object pools

        ITS SUPER IMPORTANT TO ADD THIS LINE:
        OBJ_REGISTER(<type>);
    */
    TestObject():GameObject(){
        OBJ_REGISTER(TestObject);
        health = 0;
        isround = false;
        solid = true;
    };
    /*
        Should add OBJ_REGISTER(TestObject);, but as i called the empty constructor, there is no need.
    */
    TestObject(Point pos):TestObject(){
        box.x = pos.x;
        box.y = pos.y;
        box.w = box.h = 16;
        health = 5;
        ball.x = pos.x;
        ball.y = pos.y;
        ball.r = 8;

    };

    void Update(float dt){

        box.x += speed.x * dt;
        box.y += speed.y * dt;

        if (box.x <= 0 || box.x >= SCREEN_SIZE_W || box.y <= 0 || box.y >= SCREEN_SIZE_H){
            speed = Point(0,0);
            box.x = SCREEN_SIZE_W/2;
            box.y = SCREEN_SIZE_H/2;
            health--;
            if (IsDead()){
                bear::out << "Object dead. Pool id was: " << poolIndex << "\n";
            }
        }

        if (speed.x == 0 && speed.y == 0){
            speed = Point(Geometry::toRad(rand()%360));
            speed = speed * 18.0f;
        }
    }

    void Render(){
        float maxHealth = 5.0f;
        float colorChange = (health / maxHealth) * 255.0f;
        RenderHelp::DrawCollisionBox(box,Camera::pos.x,255,colorChange,colorChange,255);
    }

    bool IsDead(){
        return health <= 0;
    };



    /*
        Unecessary methods, but declared anyways here for the sake teach
    */

    void Kill(){
        health = 0;
    }

    void NotifyCollision(GameObject * colliding){

    }

    void NotifyDamage(GameObject *object,int damage){

    };
    /*
        Force an update even outside screen?
    */
    bool canForceUpdate(){
         return false;
    };
    /*
        Force an render even outside screen?
    */
    bool canForceRender(){
        return false;
    };
    /*
        If return is 0, the game will render in perspective on y axis
    */
    int hasPerspective(){
        return 0;
    };
    /*
        This is called once the object is allocd in the pool
        so now you can handle safely "this" references.
    */
    void NotifyInPool(void *pool){
        bear::out << "Object allocd on pool. pool index: " << poolIndex << "\n";
    };



    private:
        Point speed;
        int health;
};

class TestObject2: public GameObject{
    public:
        TestObject2():GameObject(){
            OBJ_REGISTER(TestObject2);
            health = 0;
            isround = false;
            solid = true;
        };

        TestObject2(Point pos){
            OBJ_REGISTER(TestObject2);
            box.x = pos.x;
            box.y = pos.y;
            box.w = box.h = 16;
            health = 5;
            ball.x = pos.x;
            ball.y = pos.y;
            ball.r = 8;

        };

        void Update(float dt){

            box.x += speed.x * dt;
            box.y += speed.y * dt;

            if (box.x <= 0 || box.x >= SCREEN_SIZE_W || box.y <= 0 || box.y >= SCREEN_SIZE_H){
                speed = Point(0,0);
                box.x = SCREEN_SIZE_W/2;
                box.y = SCREEN_SIZE_H/2;
                health--;
                if (IsDead()){
                    bear::out << "Object 2 dead. Pool id was: " << poolIndex << "\n";
                }
            }

            if (speed.x == 0 && speed.y == 0){
                speed = Point(Geometry::toRad(rand()%360));
                speed = speed * 13.0f;
            }
        }

        void Render(){
            float maxHealth = 5.0f;
            float colorChange = (health / maxHealth) * 255.0f;
            RenderHelp::DrawCollisionBox(box,Camera::pos.x,255,255-colorChange,255,255);
        }

        bool IsDead(){
            return health <= 0;
        };

        void NotifyInPool(void *pool){
            //bear::out << "Object 2 allocd on pool. pool index: " << poolIndex << "\n";
        };
    private:
        Point speed;
        int health;
};
