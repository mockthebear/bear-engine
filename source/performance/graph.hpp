#pragma once
#include <unordered_map>
#include "../framework/geometry.hpp"
#include "../engine/text.hpp"

class GraphBar{
    public:
        GraphBar():color({0,0,0,255}),value(0),label("<noname>"),id(0),textContent(),valueContent(),created(false){};
        void SetName(std::string nm);
        bool SetValue(float v);
        void SetColor(SDL_Color color);
        bool isValid();

        void Render(Point pos,Point size,float max);
        SDL_Color color;
        float value;
        std::string label;
        uint32_t id;
    private:
        Text textContent;
        Text valueContent;
        bool created;
};

class Graph{
    public:
        Graph();
        void clear();
        void Start(Point size,float maxV);

        uint32_t AddBar(std::string name,SDL_Color color,float value);
        bool UpdateBar(uint32_t id,float value);

        void Update(float dt){};
        void Render(Point pos);

        void UpdateUi();
    private:
        Text maxWidgetContent;
        uint32_t barCounter;
        std::map<uint32_t,GraphBar> bars;
        Point size;
        float maxValue;
};
