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
    private:
        PathFind astar;
        std::vector<Point> path;
        std::vector<Rect> staticBlock;
        float volume;
        Sprite bg;
        Text message;
        int Group;
};
#endif
