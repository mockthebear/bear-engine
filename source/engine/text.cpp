#include "text.hpp"
#include "../settings/configmanager.hpp"
#include "../framework/gamefile.hpp"
#include "../framework/dirmanager.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/utils.hpp"
#include "../performance/console.hpp"
#include "../framework/debughelper.hpp"
#include "renderhelp.hpp"
#include "gamebase.hpp"
std::unordered_map<std::string, TTF_Font*> Text::assetTable;
std::unordered_map<std::string, CustomFont*> Text::customTable;

CustomFont::CustomFont(std::string letterpositions){
    oldAlpha = 255;
    loaded = false;
    GameFile *fp = new GameFile(letterpositions,true);
    if (fp->IsOpen()){
        fp->Cache();
        char *cached = fp->GetCache();
        std::string content = std::string(cached);





        utils::ReadUntil(content,"=>");
        std::string spriteName = utils::ReadUntil(content,"<=");
        std::string data = utils::ReadUntil(content,"=>end<=");
        while (true){
            std::string line = utils::ReadUntil(data,';');
            if (line.size() > 3){
                char c = utils::ReadChar(line);
                while (c == '\n' || c == '\r'){
                    c = utils::ReadChar(line);
                }
                utils::ReadChar(line);
                if (c == '%'){
                    //Particular case
                    c = utils::GetNumber(line,true);
                }
                int xx= utils::GetNumber(line,true);
                int yy= utils::GetNumber(line,true);
                int ww= utils::GetNumber(line,true);
                int hh= utils::GetNumber(line,true);
                int paddX= utils::GetNumber(line,true);
                int paddY= utils::GetNumber(line,true);
                int pddin= utils::GetNumber(line,true);
                int xoffset = utils::GetNumber(line,true);
                int resetX = utils::GetNumber(line,true);
                Letters[(unsigned char)c] = Letter(xx,yy,ww,hh,paddX,paddY,pddin, xoffset,resetX);

            }else{
                break;
            }
        }
        delete []cached;
        loaded = true;
        fp->Close();
        delete fp;
    }else{
        bear::out << "[CustomFont::CustomFont] Cannot load [" <<letterpositions<< "]\n";
        delete fp;
    }
}
CustomFont::~CustomFont(){
    Letters.clear();
}

Point CustomFont::GetSizes(std::string str_){
    Point p;
    p.x = 0;
    p.y = 0;
    if (!loaded)
        return p;



    unsigned char *str = (unsigned char *)str_.c_str();
    int x=0;
    int y=0,lineY=0;


    for (unsigned int i=0;i<str_.size();i++){
        if (!Letters[str[i]].valid){
            x += 6;
        }else{
            Letter l = Letters[str[i]];


            lineY = std::max(lineY,l.h);

            x += l.w + l.padx;
            y += l.pady;
            if (l.resetX){

                x = 0;
            }
        }
        p.x = std::max((int)p.x,x);
        p.y = std::max(std::max((int)p.y,y),lineY);
    }
    return p;
}
Point CustomFont::RenderCentered(std::string c,PointInt p,int alpha,TextCenterStyle align){
    Point sz = GetSizes(c);
    return Render(c,p.x- (align&TEXT_CENTER_HORIZONTAL ? sz.x/2.0f : 0),p.y - (align&TEXT_CENTER_VERTICAL ? sz.y/2.0f : 0) ,alpha);
}

Point CustomFont::Render(std::string str_,int x_,int y_,int alpha){
    Point p;
    p.x = 0;
    p.y = 0;
    if (!loaded)
        return p;



    unsigned char *str = (unsigned char *)str_.c_str();
    int beginx = x_;
    int beginy = y_;
    int x=0;
    int y=0,lineY=0;
    if (oldAlpha != alpha){
        oldAlpha = alpha;
        sp.SetAlpha(alpha);
    }

    for (unsigned int i=0;i<str_.size();i++){
        if (!Letters[str[i]].valid){
            x += 6;
        }else{
            Letter l = Letters[str[i]];

            sp.SetClip(l.x,l.y,l.w,l.h);
            lineY = std::max(lineY,l.h);
            sp.Render(PointInt(beginx+x+l.xoffset,beginy+y+l.yoffset));
            x += l.w + l.padx;
            y += l.pady;
            if (l.resetX){

                x = 0;
            }
        }
        p.x = std::max((int)p.x,x);
        p.y = std::max(std::max((int)p.y,y),lineY);
    }
    return p;
}





Text::Text(std::string textArg,int sizeArg,SDL_Color colorArg):Text("engine:default.ttf", sizeArg,TEXT_SOLID,textArg, colorArg){

}
Text::Text(std::string fontfilep, int fontsize,TextStyle stylep, std::string textp, SDL_Color colot,int x,int y):Text(){
    emptyText = false;
    font = nullptr;
    texturespr = nullptr;
    text = textp.c_str();
    bg = {0,0,0,0};
    size = fontsize;
    style = stylep;
    color = colot;
    texture = nullptr;

    std::string ftnm = fontfilep;

    InternalSetFont(ftnm);
    //Not resource tweaks

    //m_renderData->color[3] = 1.0f;

    RemakeTexture();


}

void Text::InternalSetFont(std::string ftnm){
    bool tweaks = true;
    char buff[20];
    int fontsize = size;

	sprintf(buff,"%d",fontsize);
	std::string aux = ftnm;
    if (!ResourceManager::IsValidResource(aux)){
        isWorking = false;
        tweaks = false;
    }
    std::string oName = ftnm;
    ftnm = ftnm+buff;


    if (assetTable[ftnm]){
        font =assetTable[ftnm];
        isWorking = true;
    }else{
        if (tweaks){
            SDL_RWops* rw = ResourceManager::GetInstance().GetFile(oName);
            font = TTF_OpenFontRW(rw,true,fontsize);
        }else{

            font = TTF_OpenFont(DirManager::AdjustUserPath(oName).c_str(), fontsize);
        }
        isWorking = true;
        fontfile = ftnm;
        if (font != NULL){
            assetTable[ftnm] = font;
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot load font [%s] because %s",ftnm.c_str(),SDL_GetError()));
            isWorking = false;
        }
    }
}


Text::Text(std::string fontfilep, std::string textp,int x,int y):Text(){
    isWorking = false;
    font = nullptr;
    texturespr = nullptr;
    text = textp;
    bg = {0,0,0,0};
    texture = nullptr;
    if (customTable[fontfilep]){
        texturespr = customTable[fontfilep];
        isWorking = true;
    }else{
        texturespr = customTable[fontfilep] = new CustomFont(fontfilep);
        isWorking = true;
    }
    Point p = texturespr->GetSizes(text);
    m_renderData.translation = Point(x,y);
    m_renderData.SetClip(Rect(0.0f,0.0f,p.x,p.y), p);
}

Text::~Text(){}

void Text::Close(){
    if (texture){

        //
        isWorking = false;
        font = nullptr;
        texturespr = nullptr;
    }
}
void Text::Render(int cameraX,int cameraY,TextRenderStyle renderStyle){

    if (!isWorking || emptyText){

        return;
    }
    if (renderStyle != 0){
        if (renderStyle & TEXT_RENDER_TOPRIGHT){
            cameraX -= GetWidth();
        }
        if (renderStyle & TEXT_RENDER_BOTTOMLEFT){
            cameraY -= GetHeight();
        }
        if (renderStyle & TEXT_RENDER_CENTERHOR){
            cameraX -= GetWidth()/2;
        }
        if (renderStyle & TEXT_RENDER_CENTERVER){
            cameraY -= GetHeight()/2;
        }
    }

    if (font){
        if (!texture){
            RemakeTexture();
        }

        if (!texture){
            RemakeTexture();
            if (!texture){
                return;
            }
        }

        m_renderData.SetPosition(Point(cameraX,cameraY));
        m_vertexes->AddRect(m_renderData);

        Painter::RenderTexture(texture.get(),m_vertexes);

        DisplayGlError("Text::Render");

    }else if (texturespr){
        Point p = texturespr->Render(text,m_renderData.translation.x,m_renderData.translation.y, 255); //todo: alpha
        m_renderData.SetClip(Rect(0.0f,0.0f,p.x,p.y), p);
    }
}


void Text::SetText(std::string str){
    if (!font && texturespr){
        text = str;
        Point p =  texturespr->GetSizes(str);
        m_renderData.SetClip(Rect(0.0f,0.0f,p.x,p.y), p);
        return;
    }
    text = str;
    RemakeTexture();
}
void Text::SetAlpha(int alpha_p){

    m_renderData.defaultColor[3] = alpha_p/255.0f;
}
void Text::SetColor(SDL_Color col ){
    if (!isWorking)
        return;
    if (!font)
        return;
    color = col;
    RemakeTexture();
}
void Text::SetStyle(TextStyle syle){
    if (!font)
        return;
    style = syle;

}
void Text::SetFontSize(int ftsz){
    if (!font)
        return;
    std::string ftnm = fontfile;
    char buff[10];
	sprintf(buff,"%d",ftsz);
    ftnm = ftnm+buff;

    if (assetTable[ftnm]){
        font =assetTable[ftnm];
    }else{
        fontfile = ftnm;
        font = TTF_OpenFont(fontfile.c_str(), ftsz);
        if (font != NULL){
            assetTable[ftnm] = font;
        }
    }
    if (font){
        RemakeTexture();
    }
}

void Text::cpy(Text *t){
    text        = t->GetText();
    font        = t->font;
    texturespr  = t->texturespr;
    isWorking   = t->isWorking;
    bg          = t->bg;
    size        = t->size;
    style       = t->style;
    color       = t->color;
}

void Text::Clear(){
    for (auto &it : assetTable){
        TTF_CloseFont(it.second);
    }
    assetTable.clear();

}

//private
void Text::RemakeTexture(bool Destory){
    if (!isWorking)
        return;
    if (text == ""){
        m_renderData.GetClip().w = 0.0f;
        emptyText = true;
        return;
    }else{
        emptyText = false;
    }
    if (Game::GetInstance()->isClosing)
        return;
    if (!font && texturespr){
        Point p =  texturespr->GetSizes(text);
        m_renderData.SetClip(Rect(0.0f,0.0f,p.x,p.y), p);
        return;
    }


    SDL_Surface *surf=nullptr;
    if (!font){
        bear::out << "[TXT:RemakeTexture] there is no font\n";
        return;
    }
    if (style == TEXT_SOLID){
        surf = TTF_RenderText_Solid(font,text.c_str(),color);
    }else if(style == TEXT_SHADED){
        surf = TTF_RenderText_Shaded(font, text.c_str(),color, bg);
    }else if(style == TEXT_BLENDED){
        surf = TTF_RenderText_Blended(font,text.c_str(),color);
    }

    if (surf){

        BearTexture *b2 = RenderHelp::SurfaceToTexture(surf,aliasing);
        SDL_FreeSurface(surf);
        if (b2){
            texture = std::shared_ptr<BearTexture>(b2,[](BearTexture* bher)
            {
                bher->ClearTexture();
                delete bher;
            });
            m_renderData.SetClip(Rect(0.0f,0.0f,texture->size_w,texture->size_h), Point(texture->texture_w,texture->texture_h));
        }

    }else{
        bear::out << "[Text:RemakeTexture] Surface not loaded "<<SDL_GetError()<<" ("<<text<<")\n";
    }
}
