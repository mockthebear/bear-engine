#include "window.hpp"
#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

Window::Window(Rect dimensions):UIBase(){
    UI_REGISTER(Window);
    o_pos = Point(dimensions.x,dimensions.y);
    box = dimensions;
    mother = nullptr;
    OnMousePress = [=](UIBase*w,int button,Point pos){
        SetFocused(true);
    };
}
Window::~Window(){

}
void Window::Input(){
    UIBase::Input();
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y) && InputManager::GetInstance().MousePress(1) ){
        SetFocused(true);
    }
    if (!IsInside(pos.x,pos.y) &&  (InputManager::GetInstance().MouseRelease(1) || InputManager::GetInstance().MousePress(1)) ){
        SetFocused(false);
    }
}



void Window::Render(Point where){
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,style.bg[0] + (focused ? 0 : -20),style.bg[1] + (focused ? 0 : -20),style.bg[2] + (focused ? 0 : -20),style.bg[3] + (focused ? 0 : -20));
    where.x = box.x;
    where.y = box.y;
    UIBase::Render(where);
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    RenderHelp::DrawSquareColorA(pos.x,pos.y,3,3,255,0,0,100);
}
/*

TextInput::TextInput(int x,int y,std::string str,Style sta,Window *mommy,int ind,char *num){
    box.x = x;
    box.y = y;
    st = sta;
    caddr = num;
    mother = mommy;
    text = str;

    focused = false;
    SDL_Color col = {st.fg[0],st.fg[1],st.fg[2]};
    txt = new Text(st.fontfile,st.fontSize,st.txtstyle,text,col,0,0);
    txt->SetScaleX(ConfigManager::GetInstance().GetUIscale());
    txt->SetScaleY(ConfigManager::GetInstance().GetUIscale());
    box.w = 110;
    box.h = 20;

    ID = ind;
    sp = NULL;

    txt->SetText(caddr);

    type_input = 2;
    csize=0;



}

TextInput::TextInput(int x,int y,std::string str,Style sta,Window *mommy,int ind,int *num){
    box.x = x;
    box.y = y;
    st = sta;

    printf("%d\n",*num);
    addr = num;
    mother = mommy;
    text = str;

    focused = false;
    SDL_Color col = {st.fg[0],st.fg[1],st.fg[2]};
    txt = new Text(st.fontfile,st.fontSize,st.txtstyle,text,col,0,0);
    txt->SetScaleX(ConfigManager::GetInstance().GetUIscale());
    txt->SetScaleY(ConfigManager::GetInstance().GetUIscale());
    box.w = 110;
    box.h = 20;
    saveat = *num;
    ID = ind;
    sp = NULL;
    std::stringstream aux;
    aux << text << saveat;
    if (addr!= NULL)
        (*(addr)) = saveat;
    txt->SetText(aux.str());

    type_input = 1;

}

TextInput::~TextInput(){
    delete txt;
}




void TextInput::Render(){
    //RenderHelp::DrawSquareColorA(499,149,302,152,255,100,100);
    //RenderHelp::DrawSquareColorA(500,150,300,150,195,205,200);
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,focused ? 250 : 100,focused ? 200 : 100,focused ? 250 : 100);
    txt->SetScaleX(ConfigManager::GetInstance().GetUIscale());
    txt->SetScaleY(ConfigManager::GetInstance().GetUIscale());
    txt->Render(box.x,box.y);
}

void TextInput::Update(float dt){
    int mx = InputManager::GetInstance().GetMouseX();
    int my = InputManager::GetInstance().GetMouseY();
    if (mx >= box.x and mx <= box.x+box.w*ConfigManager::GetInstance().GetUIscale()
        and my >= box.y and my <= box.y+box.h*ConfigManager::GetInstance().GetUIscale()){
        if (InputManager::GetInstance().MousePress(LEFT_MOUSE_BUTTON)){
            mother->OnPressed(this);
        }
    }

    if (focused){

        if (type_input == 1){
            for (int i=48;i<=57;i++){
                if (InputManager::GetInstance().KeyPress(i)){
                    int add = saveat;
                    char beef[] = "        ";
                    if (add == 0){
                        sprintf(beef,"%d",(i-48));
                    }else{
                        sprintf(beef,"%d%d",add,(i-48));
                    }
                    sscanf(beef,"%d",&add);

                    saveat = add;
                }
            }
            if (InputManager::GetInstance().KeyPress(8)){
                int add = saveat;
                char beef[] = "        ";
                sprintf(beef,"%d",add);
                if (add > 10){
                    int len = strlen(beef);
                    beef[len-1] = '\0';
                }else{
                    sprintf(beef,"%d",0);
                }
                sscanf(beef,"%d",&add);
                saveat = add;

            }
            std::stringstream aux;
            aux << text << saveat;
            if (addr!= NULL)
                (*(addr)) = saveat;
            txt->SetText(aux.str());
        }else{
            for (int i=97;i<=122;i++){
                if (InputManager::GetInstance().KeyPress(i)){
                    caddr[csize] = i;
                    csize++;
                }
            }
            for (int i='0';i<='9';i++){
                if (InputManager::GetInstance().KeyPress(i)){
                    caddr[csize] = i;
                    csize++;
                }
            }
            if (InputManager::GetInstance().KeyPress(32)){
                    caddr[csize] = ' ';
                    csize++;
                }
            if (InputManager::GetInstance().KeyPress(8)){
                csize--;
                csize = std::max(0,csize);
            }
            caddr[csize] = '\0';
            txt->SetText(caddr);
        }

    }

}


NumberController::NumberController(int x,int y,Style st,Window *mom,int ID,int *num,int mina,int maxa){
    box.x = x;
    box.y =y;
    box.w = 64;
    box.h = 32;
    mother = mom;
    addr = num;
    max = maxa;
    min = mina;
    n = *addr;
    SDL_Color col = {st.fg[0],st.fg[1],st.fg[2]};
    std::stringstream S;
    S << n;
    txt = new Text(st.fontfile,st.fontSize,st.txtstyle,S.str(),col,0,0);
}

NumberController::~NumberController(){
}
void NumberController::Render(){
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,100,100,100,255);
    RenderHelp::DrawSquareColorA(box.x+2,box.y+4,32,24,150,150,150,255);

    RenderHelp::DrawSquareColorA(box.x+36,box.y+4,24,8,255,150,150,255);
    RenderHelp::DrawSquareColorA(box.x+36,box.y+20,24,8,150,255,150,255);
    std::stringstream S;
    S << n;
    txt->SetText(S.str());
    txt->Render(box.x+18-txt->GetWidth()/2,box.y+8);
}
void NumberController::Update(float dt){
    if (!focused){

        if (InputManager::GetInstance().IsKeyDown(SDLK_LEFT)){
            n--;
        }
        if (InputManager::GetInstance().IsKeyDown(SDLK_RIGHT)){
            n++;
        }
        n = std::max(n,min);
        n = std::min(n,max);

        *addr = n;

    }
}
void NumberController::SetSprite(std::string sprpath){
};


MiniTextInfo::MiniTextInfo(){
    box.x=0;
    box.y=0;
    box.w = 0;
    box.h = 0;
    setText("");
}

MiniTextInfo::MiniTextInfo(int x,int y,std::string text){
    box.x=x;
    box.y=y;
    box.w = 0;
    box.h = 0;
    setText(text);
}

MiniTextInfo::~MiniTextInfo(){
    Lines.clear();
}

void MiniTextInfo::Render(){
    if (Blocked)
        return;
    RenderHelp::DrawSquareColorA(box.x-2,box.y-2,box.w+19,box.h+19,88,76,39,255);
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w+16,box.h+16,188,176,139,255);
    int amount = 0;
    for (auto &it : Lines){
        it->Render(box.x+8,box.y+8+amount);
        amount += it->GetHeight()+2;
    }

}
void MiniTextInfo::Update(float dt){

}
void MiniTextInfo::OnPressed(WindowComponent *component){

}
void MiniTextInfo::setText(std::string text){
    Lines.clear();
    Lines.emplace_back(new Text("font/mainfont.burrt",text));
    int maxX=0,maxY=0;
    for (auto &it : Lines){
        maxX = std::max((int)it->GetWidth(),maxX);
        maxY += it->GetHeight();
    }
    box.w = maxX+16;
    box.h = maxY+16;
}
*/
