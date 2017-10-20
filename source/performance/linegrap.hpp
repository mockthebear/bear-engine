#pragma once
#include "../framework/geometry.hpp"
#include "../engine/text.hpp"
#include <vector>

/** \brief An line graph plot thing
 *
 */

class LineGraph{
    public:
        LineGraph();
        /** \brief Constructor
         *
         * This continuosly shows the graph lines.
         * Once the limit is reached, the first value
         * is poped, and the next is addedon the end, scrolling the
         * whole sequence to the left
         *
         * \param X and Y cordinates of the size
         * \param  Amount of data stored
         *
         */

        LineGraph(Point size,uint16_t dataCount);
        ~LineGraph();
        /** \brief Insert an data
         *
         * \param Value
         *
         */

        void AddData(float data);
        /** \brief Display in screen the graph
         *
         * \param The position on screen
         *
         */

        void Render(Point pos);
    private:
        Text tmax,tmin,tlast;
        float min,max;
        Point size;
        uint16_t dataCount;
        float lastY;
        std::vector<float> data;

};
