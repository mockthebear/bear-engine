#include "graph.hpp"
#include "../engine/renderhelp.hpp"



void GraphBar::SetName(std::string nm){
    created = true;
    label = nm;
    textContent = Text(nm,12,color);
    valueContent = Text("0",12,{100,100,100,255});
    valueContent.SetRotation(90);
}

bool GraphBar::SetValue(float v){
    float old = value;
    if (!created){
        old = -1;
    }
    created = true;
    value = v;
    valueContent.SetText(utils::format("%.2f",v));
    return old != v;

}
void GraphBar::SetColor(SDL_Color argColor){
    color = argColor;
    if (created){
        textContent.SetColor(color);
    }
}
bool GraphBar::isValid(){
    return created;
}

void GraphBar::Render(Point pos,Point size,float max,int width){
    if (!created){
        return;
    }
    float barSize = (value / max)*size.y;
    RenderHelp::DrawSquareColor(Rect(pos.x + id * (width +4) ,pos.y+size.y-barSize,width,barSize), color.r,color.g,color.b,color.a);
    textContent.Render(pos.x + id * (width +4),pos.y+size.y,TEXT_RENDER_TOPLEFT);
    if (barSize > valueContent.GetWidth()){
        valueContent.Render(pos.x + id * (width +4),pos.y+size.y-barSize + valueContent.GetWidth()/2,TEXT_RENDER_TOPLEFT);
    }
}


Graph::Graph(){
    bars.clear();
    barCounter = 0;
    width = 32;

}

void Graph::clear(){
    bars.clear();
    barCounter = 0;
    maxWidgetContent = Text("?",12,{100,100,100,255});
}


void Graph::Start(Point argSize,float maxV,bool avg){
    size = argSize;
    width = maxV;
    useAvg = avg;
}


bool Graph::UpdateBar(uint32_t id,float value){
    if (!bars[id].isValid()){
         return false;
    }

    if (useAvg){
        value = (bars[id].value+value)/2.0f;
    }
    bars[id].SetValue(value);
        UpdateUi();

    return true;
}
void Graph::UpdateUi(){
    maxValue = 0;
    for (auto &it : bars){
        maxValue = std::max(maxValue,it.second.value);
    }
    maxWidgetContent.SetText(utils::format("%.2f",maxValue));
}
uint32_t Graph::AddBar(std::string name,SDL_Color color,float value){
    GraphBar bar;
    bar.SetColor(color);
    bar.SetName(name);
    bar.SetValue(value);
    bar.id = barCounter;
    bars[barCounter] = bar;
    UpdateUi();
    size.x = std::max(size.x,width + barCounter * (width+4.0f));
    barCounter++;
    return barCounter-1;
}

void Graph::Render(Point pos){
    // inner box
    RenderHelp::DrawSquareColorA(pos.x,pos.y,size.x,size.y,100,100,100,100,false);



    for (auto &it : bars){
        it.second.Render(pos,size,maxValue,width);
    }



    // outside box
    RenderHelp::DrawSquareColorA(pos.x,pos.y,size.x,size.y,100,100,100,255,true);
    maxWidgetContent.Render(pos.x,pos.y,TEXT_RENDER_TOPRIGHT);

}
