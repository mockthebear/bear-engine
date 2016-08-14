#ifndef TitleH
#define TitleH

#include "state.hpp"
#include "../performance/console.hpp"
class Title: public State{
    public:
        Title();
        ~Title();
        void Update(float);
        void Render();
        void Input();
        void Begin();
    private:
        float volume;
        Sprite bg;
        Text message;
        int Group;
};
#endif
