#include "../engine/genericstate.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <list>


#ifndef STATEHH
#define STATEHH

class State: public GenericState{
    public:
        State(){};
        virtual ~State(){};
        virtual void Update(float dt) =0;
        virtual void Render() =0;
        virtual void Begin() =0;
        float PlayerSpeed;
        int currentLevel;

};

#endif // STATEH

