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
            sp.Render(beginx+x+l.xoffset,beginy+y+l.yoffset);
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
    angle = 0;
    emptyText = false;
    font = nullptr;
    texturespr = nullptr;
    box.x = x;
    box.y = y;
    text = textp.c_str();
    bg = {0,0,0,0};
    size = fontsize;
    style = stylep;
    color = colot;
    texture = nullptr;
    alpha=255;
    std::string ftnm = fontfilep;

    InternalSetFont(ftnm);
    //Not resource tweaks




    scaleY=scaleX=1;
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
    angle = 0;
    isWorking = false;
    font = nullptr;
    texturespr = nullptr;
    box.x = x;
    alpha=255;
    box.y = y;
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
    box.w = p.x;
    box.h = p.y;
    scaleY=scaleX=1;




}
Text::~Text(){
}

void Text::Close(){
    if (texture){
        #ifndef RENDER_OPENGL
        SDL_DestroyTexture( texture );
        texture = nullptr;
        #endif // RENDER_OPENGL
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
        #ifndef RENDER_OPENGL
        SDL_Rect dimensions2;
        double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
        double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
        dimensions2.x = box.x*scaleRatioW+cameraX*scaleRatioW+ ScreenManager::GetInstance().GetOffsetW();
        dimensions2.y = box.y*scaleRatioH+cameraY*scaleRatioH;
        dimensions2.h = box.h*scaleRatioH*scaleY;
        dimensions2.w = box.w*scaleRatioW*scaleY;


        if (texture){
            SDL_SetTextureAlphaMod(texture,alpha);
            if (SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,nullptr,&dimensions2,static_cast<double>(angle),nullptr,SDL_FLIP_NONE)){
                RemakeTexture();
            }
        }
        #else
        if (!texture){
            RemakeTexture();
            if (!texture){
                return;
            }
        }

        RenderHelp::RenderTexture(texture.get(),Point(box.x+cameraX,box.y+cameraY),Rect(0,0,-1,-1),angle,Point(1,1),SDL_FLIP_NONE,BearColor(1.0,1.0,1.0,alpha/255.0f));
        DebugHelper::DisplayGlError("Text::Render");
        #endif
    }else if (texturespr){
        Point p = texturespr->Render(text,box.x+cameraX,box.y+cameraY,alpha);
        box.w = p.x;
        box.h = p.y;
    }
}


void Text::SetText(std::string str){
    if (!font && texturespr){
        text = str;
        Point p =  texturespr->GetSizes(str);
        box.w = p.x;
        box.h = p.y;
        return;
    }
    text = str;
    RemakeTexture();
}
void Text::SetAlpha(int alpha_p){

    alpha = alpha_p;

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
    box         = t->box;
    isWorking   = t->isWorking;
    alpha       = t->alpha;
    angle       = t->angle;
    bg          = t->bg;
    size        = t->size;
    style       = t->style;
    color       = t->color;
    scaleY      = t->scaleY;
    scaleX      = t->scaleX;



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
        box.w = 0;
        emptyText = true;
        return;
    }else{
        emptyText = false;
    }
    if (Game::GetInstance()->isClosing)
        return;
    if (!font && texturespr){
        Point p =  texturespr->GetSizes(text);
        box.w = p.x;
        box.h = p.y;
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
        #ifndef RENDER_OPENGL
        if (aliasing)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
        texture = SDL_CreateTextureFromSurface(BearEngine->GetRenderer(),surf);
        Uint32 format;
        int acess,w,h;
        SDL_QueryTexture(texture, &format,&acess,&w,&h);
        box.h=h;
        box.w=w;
        if (aliasing)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
        SDL_FreeSurface(surf);
        #else
        BearTexture *b2 = RenderHelp::SurfaceToTexture(surf,aliasing);
        SDL_FreeSurface(surf);
        if (b2){
            texture = std::shared_ptr<BearTexture>(b2,[](BearTexture* bher)
            {
                bher->ClearTexture();
                delete bher;
            });
            box.w = texture->size_w;
            box.h = texture->size_h;
        }
        #endif // RENDER_OPENGL
    }else{
        bear::out << "[Text:RemakeTexture] Surface not loaded "<<SDL_GetError()<<" ("<<text<<")\n";
    }
}
