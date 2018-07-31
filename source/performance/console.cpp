#include "console.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "../framework/dirmanager.hpp"
#ifdef __ANDROID__
#define APPNAME "BearEngine"
#include <android/log.h>
#endif
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
    logFile=false;
    failFile = file != "" ? true : false;
    if (file != ""){
        myfile.Open(file);
        logFile = true;
        failFile = false;
    }
}
Console::~Console(){
    if (hasStarted){
    for (int i=0;i<m_lines;i++){
        delete m_line[i];
    }
    delete [] m_line;
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
    hasStarted = true;
    for (auto &it : StoredLines){
        AddText(it,true);
    }
    StoredLines.clear();
    if (failFile){
        failFile = false;
        AddText("Failed to openwrite the file");
    }

}
bool Console::AddText(std::string _str,bool onlyGraphic){
    if (hasStarted){
        if (!onlyGraphic){
            if (logFile)
                myfile << _str << "\r\n";
            std::cout << _str<<"\n";

            #ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "%s\n",_str.c_str());
            #endif
        }
        if (m_lines <= counter){
            for (int i=0;i<m_lines;i++){
                std::string str = " ";
                if (i+1 < m_lines){
                    str = m_line[i+1]->GetText();
                }
                if (!Game::Crashed)
                    m_line[i]->SetText(str);
            }
            if (!Game::Crashed)
                m_line[m_lines-1]->SetText(_str);
        }else{
            if (!Game::Crashed)
                m_line[counter]->SetText(_str);
            counter++;
        }
    }else{
        if (onlyGraphic)
            return false;
        StoredLines.emplace_back("[N.S] " + _str);
        std::cout << "[Console:Not started] "<<_str<<"\n";
        if (logFile)
            myfile << "[Console:Not started] "<<_str<<"\r\n";



    }
    return true;

}

void Console::Render(Point pos){
    RenderHelp::DrawSquareColor(pos.x+box.x,pos.y+box.y,box.w,box.h,0,0,0,100);
    box.w = 100;
    if (hasStarted){
        for (int i=0;i<m_lines;i++){
            m_line[i]->Render(pos.x+box.x,pos.y+box.y + m_line[0]->GetHeight()*i);
            box.w = std::max(box.w, m_line[i]->GetWidth());
        }
    }
}

