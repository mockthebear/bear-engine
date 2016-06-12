#include "console.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "../framework/dirmanager.hpp"
#include <iostream>
#include <sstream>


Console& Console::GetInstance(bool writeFile){
    std::string name = writeFile ? "console.txt" : "";
    static Console teste(25,300,name);
    return teste;
}

Console::Console(int lines_, int width,std::string file){
    box.x = 0;
    box.y = 0;
    box.w = width;
    m_lines = lines_;
    counter = 0;
    hasStarted = false;
    m_startedUi = false;
    logFile=false;
    keyword = "[Console]";
    failFile = file != "" ? true : false;
    if (file != ""){
        if (myfile.Open(file)){
            hasStarted = true;
            logFile = true;
            failFile = false;
        }

    }
}
Console::~Console(){
    if (m_startedUi){
        for (int i=0;i<m_lines;i++){
            delete m_line[i];
        }
        delete m_line;
    }
    myfile << "Engine closed\n";
    CloseOutput();
}
void Console::CloseOutput(){
    if (logFile){
        myfile.Close();
    }
}
void Console::Begin(){
    m_line = new Text*[m_lines];
    for (int i=0;i<m_lines;i++){
        m_line[i] = new Text("engine:default.ttf",16,TEXT_SOLID," ", {255,255,255});
    }
    box.h = m_line[0]->GetHeight()*m_lines;
    if (failFile){
        failFile = false;
        AddText("Failed to openwrite the file");
    }
}
bool Console::AddText(std::string str){
    if (hasStarted){
        if (logFile)
            myfile << str << "\n";
        if (keyword != "")
            std::cout << keyword << " ";
        std::cout << str<<"\n";
        if (m_startedUi){
            if (m_lines <= counter){
                for (int i=0;i<m_lines;i++){
                    std::string str = " ";
                    if (i+1 < m_lines){
                        str = m_line[i+1]->GetText();
                    }
                    m_line[i]->SetText(str);
                }
                m_line[m_lines-1]->SetText(str);
            }else{
                m_line[counter]->SetText(str);
                counter++;
            }
        }
    }else{
        if (keyword != "")
            std::cout << keyword << " ";
        std::cout << "[Console:Not started] "<<str<<"\n";
        if (logFile)
            myfile << "[Console:Not started] "<<str<<"\n";
    }
    return true;

}

void Console::Render(Point pos){
    RenderHelp::DrawSquareColorA(pos.x+box.x,pos.y+box.y,box.w,box.h,0,0,0,100);
    if (m_startedUi){
        for (int i=0;i<m_lines;i++){
            m_line[i]->Render(pos.x+box.x,pos.y+box.y + m_line[0]->GetHeight()*i);
        }
    }
}

