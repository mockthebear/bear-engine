#pragma once
#include "../framework/geometry.hpp"
#include "../engine/text.hpp"
#include <vector>


class LineGraph{
    public:
        LineGraph();
        LineGraph(Point size,uint16_t dataCount);
        ~LineGraph();

        void AddData(float data);
        void Render(Point pos);
    private:
        Text tmax,tmin,tlast;
        float min,max;
        Point size;
        uint16_t dataCount;
        float lastY;
        std::vector<float> data;

};
