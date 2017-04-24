#ifndef OBJH
#define OBJH

#include "../framework/geometry.hpp"
#include "../framework/typechecker.hpp"
#include <string>
#include <typeinfo>
#include <iostream>

enum ObjType{
    OBJ_NULL=0,
    //system
    OBJ_STILLANIMATION,
    OBJ_ANIMATEDTEXT,
    OBJ_CREATURE,
    OBJ_MONSTER,

    OBJ_SELECTABLE,
    //Blocks
    OBJ_COLLISION,
    OBJ_COLLISIONT,
    OBJ_COLLISIONTT,
    OBJ_ACTIONBLOCK,

    //Item
    OBJ_MAPITEM,
    OBJ_ITEM,
    OBJ_POTION,
    OBJ_COIN,
    OBJ_BOX,
    OBJ_MAPBOX,

    //Creatures
    OBJ_PLAYER,
    OBJ_SPIDER,
    OBJ_RANGED,
    OBJ_PROJECTILE,
    OBJ_TRAP,
    OBJ_BOSS,
    OBJ_PARTICLE,

    //Blegh
    OBJ_ENEMY,
    OBJ_END,

};

#define REGISTER_GETSETTER(Nm,type,var) type Get ## Nm (){return var;}; void Set ## Nm(type arg){ var = arg;}
#define OBJ_REGISTER(Type) hashIt(Types::Get<Type>());

/**
 * @brief Basic object class
 *
 * This class will be the base for any object in the game
 * Note that this class is almost an fully virtual class
 *
 * The engine have an type checker thing. So, when you define an object of the type "Player" and a type Monster
 * with the pointer you can know what object is the player and what is the monster.\n
 * To this you may have to check the class Types and this one until the end.
 *
 * On your constructor you have to startup like this:
 @code
 // Player -> GameObject
 Player::Player(){
    OBJ_REGISTER(Player);
 }
 @endcode
 *This is the constructor of the local "instance" of the Types thing\n
 *<b>DONT FORGET TO SET THIS IN ALL CONSTRUCTORS</b>
 *
 * To Types thing, you can check the fucntion GameObject::Is
 */
class GameObject{
    public:
    /**
        @brief Here you update the object
    */
    virtual void Update(float)=0;
    public:
    /**
        @brief Destructor
    */
    virtual ~GameObject(){};
    /**
        @brief Where you will render everything
    */
    virtual void Render()=0;
    /**
        *Check if the object is dead.\n
        *Used inside the engine\n
        *<b>Must be defined<b/>
        @return true if is dead
    */
    virtual bool IsDead()=0;
    /**
        Force an object to be set as dead
    */
    virtual void Kill()=0;

    /**
        *Check if the object is inside the screen.\n
        *Used inside the engine\n
        *<b>Must be defined<b/>
        @return true if is dead
    */
    virtual bool IsInScreen(Point& p)=0;
    /**
        *You can use this to say "hey, you have been touched by this object
        @param other the object who touched this one
    */
    virtual void NotifyCollision(GameObject *other)=0;
    /**
        *You can use this to say "hey, you have been touched by this object
        @param thing the parameter here is the object type HASH. Check GameObject::GetHash
        @code
            Is(int i){
                return i == OBJ_TYPE_BLA || i == GetHash();
            }
        @endcode
    */
    virtual bool Is(int thing){return thing==GetHash_internal();};
    /**
        *You can use this to say "hey, you have been damaged"
        @param other the object who touched this one
        @code
            void NotifyCollision(GameObject *other){
                if (!other)  //Always check if the pointer is not null;
                    other->NotifyDamage(this,10);
            }

        @endcode
        *:
        @code
            void NotifyDamage(GameObject *other,int damage){
                if (!other)
                    std::cout << "Ouch! i have been damaged by an object at pos "<< other->box.x << "\n";
                health -= damage;
            }

        @endcode
    */
    virtual void NotifyDamage(GameObject *bj,int n)=0;
    /**
        *The engine have an thing to dont update object that are not inside the Camera.
        *But you can force then to be updated anyway
    */
    bool canForceUpdate(){return false;};
    /**
        *More about the perspegtive/depth can be explained here: GenericState::Map\n
        *The thing you need to know about the perspective here, is when set to 0, the depth will be the
        * equal to box.y.
    */
    int hasPerspective(){return 0;};
    bool canForceRender(){return false;};

    /**
        *<b>DONT REPLACE THIS</b>
    */
    bool IsHash(int i){return i == GetHash_internal();};
    /**
        *<b>DONT REPLACE THIS</b>
    */
    int GetHash(){return GetHash_internal();};

    /**
        *Can be used as an depth var. Use as you wish
    */
    int Depth;
    /**
        *Used to check collision.
        *Actually means the position and the size of the object
    */
    Rect box;
    /**
        This can be used to make a round object. You set the x,y and radius
    */
    Circle ball;
    /**
        This will say to the collision manager that this object is round
    */
    bool isround;
    /**
        *This will say to the collision manager that this object is solid.
        *When not solid will be ignored.
    */
    bool solid;

    /**
        *<b>DONT REPLACE THIS</b>
    */
    void *_Data_;
    /**
        *<b>DONT REPLACE THIS</b>
    */
    int poolIndex;

    virtual void NotifyInPool(void *pool){};

    protected:
        void hashIt(int var){hash = var;};
        inline int GetHash_internal(){return hash;};
        float rotation;
        uint64_t Flags;


    private:

        int hash;


};

#endif
