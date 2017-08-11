#ifndef TitleH
#define TitleH

#include "state.hpp"
#include <stack>
#include "../performance/console.hpp"
#include "../engine/pathfind.hpp"
class Title: public State{
    public:
        Title();
        ~Title();
        void Update(float);
        void Render();
        void Input();
        void Begin();
        void Resume(){};
        void End(){};
    private:
        PathFind astar;
        std::vector<Point> path;
        std::vector<Rect> staticBlock;
        float volume;
        Sprite bg;
        Text message;
        Sound *snd;
        Sound *snd2;
        int Group;
};
#endif
