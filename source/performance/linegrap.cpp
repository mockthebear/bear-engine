#include "linegrap.hpp"
#include "../engine/renderhelp.hpp"



LineGraph::LineGraph():size(200.0f,100.0f),dataCount(32.0f){

}

LineGraph::LineGraph(Point sz,uint16_t dataN){
    size = sz;
    dataCount = dataN;
    min = 0.0f;
    max = 0.0f;

}


LineGraph::~LineGraph(){

}


void LineGraph::Render(Point pos){

    RenderHelp::DrawSquareColor(Rect(pos.x,pos.y,size.x,size.y),255,255,255,255,true);

    if (data.size() > 0){
        double spread = size.x/(double)dataCount;
        int count = 0;
        lastY = data[0];
        std::vector<LineColor> lines;
        for (auto &it : data){
            double y = pos.y+size.y-1;
            double x = pos.x;
            double bY = pos.y+size.y-1;

            y -= ((it-min)/max) * size.y;
            bY -= ((lastY-min)/max) * size.y;
            lastY = it;

            x += count * spread;

            count++;
            lines.emplace_back(LineColor(x, bY, x + spread,y));
        }
        RenderHelp::DrawLinesColor(lines);
    }
    tmax.Render(pos.x+size.x,pos.y,TEXT_RENDER_TOPLEFT);
    tmin.Render(pos.x+size.x,pos.y+size.y,TEXT_RENDER_TOPLEFT);
    tlast.Render(pos.x+size.x,pos.y+size.y/2.0f,TEXT_RENDER_TOPLEFT);

}


void LineGraph::AddData(double dataValue){
    data.emplace(data.end(),dataValue);
    min = std::min(dataValue,min);
    max = std::max(dataValue,max);
    if (data.size() >= dataCount){
        data.erase(data.begin());
        min = max;
        max = 0;
        for (auto &it : data){
            min = std::min(it,min);
            max = std::max(it,max);
        }
    }
    tmax = Text(utils::format("%f",max),14,{255,255,255,255});
    tmin = Text(utils::format("%f",min),14,{255,255,255,255});
    tlast = Text(utils::format("%f",dataValue),14,{255,255,255,255});

}
