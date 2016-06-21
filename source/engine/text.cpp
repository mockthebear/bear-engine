#include "text.hpp"
#include "../settings/configmanager.hpp"
#include "../framework/gamefile.hpp"
#include "../framework/dirmanager.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/utils.hpp"
#include "../performance/console.hpp"
#include "gamebase.hpp"
std::unordered_map<std::string, TTF_Font*> Text::assetTable;
std::unordered_map<std::string, CustomFont*> Text::customTable;

CustomFont::CustomFont(std::string letterpositions){
    loaded = false;
    GameFile *fp = new GameFile(letterpositions,true);
    if (fp->IsOpen()){
        fp->Cache();
        char *content = fp->GetCache();
        char pngfile[100];
        char data[5000];


        std::string j = content;

        int m = j.find("@");
        j = j.substr (m+1);
        if (sscanf(j.c_str(),"<%[a-z./\\0-9 ]>:%s@",pngfile,data)){
            sp = new Sprite(pngfile);
            std::string letterData = data;
            std::cout << content << " : [[[" << fp->GetSize()<<"]]]\n";
             bool find = true;
            int m=1;
            char letter[10];
            int xx=0,yy=0,hh=0,ww=0,padd=0,pddin=0,yoffset=0;
            while (find and m > 0){
                if ( (sscanf(letterData.c_str(),"%[a-zA-Z0-9*+,.-]=<%d,%d,%d,%d,%d,%d,%dx>",letter,&xx,&yy,&ww,&hh,&padd,&pddin,&yoffset))  ){
                    m = letterData.find(";");
                    if (m != -1){
                        Letters[(unsigned char)letter[0]] = std::unique_ptr<Letter>(new Letter(xx,yy,ww,hh,padd,pddin,yoffset));
                    }else{
                        break;
                    }
                    letterData = letterData.substr (m+1);
                }else{
                    find = 0;
                }
            }
        }
        loaded = true;
        delete content;
        delete fp;
    }else{
        std::cout << "[CustomFont::CustomFont] Cannot load [" <<letterpositions<< "]\n";
        delete fp;
    }
}
CustomFont::~CustomFont(){
    if (loaded)
        delete sp;
    Letters.clear();
}

Point CustomFont::GetSizes(std::string str_){
    Point p;
    p.x = 0;
    p.y = 0;
    if (!loaded)
        return p;

    unsigned char *str = (unsigned char *)str_.c_str();
    int x=0,y=0;
    for (unsigned int i=0;i<str_.size();i++){
        if (!Letters[str[i]]){
            if (str[i] == '\n'){
                y += 20;
                x = 0;
                p.x = std::max((int)p.x,x);
            }else{
                x += 6;
            }
        }else{
            Letter l = *Letters[str[i]].get();
            x+= l.w + l.pad;
        }
        p.x = std::max((int)p.x,x);
        p.y = std::max((int)p.y,y);
    }
    return p;
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
    int y=0;
    if (oldAlpha != alpha){
        oldAlpha = alpha;
        sp->SetAlpha(alpha);
    }

    for (unsigned int i=0;i<str_.size();i++){
        if (!Letters[str[i]]){
            if (str[i] == '\n'){
                y += 20;
                p.x = std::max((int)p.x,x);
                x = 0;
            }else{
                x += 6;
            }
        }else{
            Letter l = *Letters[str[i]].get();

                sp->SetClip(l.x,l.y,l.w,l.h);
                sp->Render(beginx+x+l.padin,beginy+y+l.yoffset);
                x+= l.w + l.pad;
        }
        p.x = std::max((int)p.x,x);
        p.y = std::max((int)p.y,y);
    }
    return p;
}





Text::Text(std::string text,int size,SDL_Color color):Text("engine:default.ttf", size,TEXT_SOLID,text, color){

}
Text::Text(std::string fontfilep, int fontsize,TextStyle stylep, std::string textp, SDL_Color colot,int x,int y){
    angle = 0;
    font = NULL;
    texturespr = NULL;
    box.x = x;
    box.y = y;
    text = textp.c_str();
    bg = {0,0,0,0};
    size = fontsize;
    style = stylep;
    color = colot;
    texture = NULL;
    alpha=255;
    std::string ftnm = fontfilep;
    char buff[20];
    sprintf(buff,"%d",fontsize);
    ftnm = ftnm+buff;
    //Not resource tweaks
    bool tweaks = true;
    if (fontfilep.find(":")==std::string::npos){

        isWorking = false;

        fontfilep = DirManager::AdjustAssetsPath(fontfilep);
        fontfile = fontfilep;
        tweaks = false;
    }


    if (assetTable[ftnm]){
        font =assetTable[ftnm];
        isWorking = true;
    }else{
        if (tweaks){
            font = TTF_OpenFontRW(ResourceManager::GetInstance().GetFile(fontfilep),1,fontsize);
        }else{
            font = TTF_OpenFont(fontfilep.c_str(), fontsize);
        }
        isWorking = true;
        if (font != NULL){
            assetTable[ftnm] = font;
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot load font [%s] because %s",ftnm.c_str(),SDL_GetError()));
            isWorking = false;
        }
    }


    scaleY=scaleX=1;
    RemakeTexture();


}

SDL_Texture* Text::CopyTexture(){
    SDL_Texture* ret = texture;
    RemakeTexture(false);
    return ret;
}

Text::Text(std::string fontfilep, std::string textp,int x,int y){
    angle = 0;
    isWorking = false;
    font = NULL;
    texturespr = NULL;
    box.x = x;
    alpha=255;
    box.y = y;
    text = textp;
    bg = {0,0,0,0};
    texture = NULL;
    std::string ftnm = fontfilep;
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
    if (texture){
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

void Text::Close(){
    if (texture){
        SDL_DestroyTexture(texture);
        texture = NULL;
        isWorking = NULL;
        font = NULL;
        texturespr = NULL;
    }
}
void Text::Render(int cameraX,int cameraY){

    if (!isWorking){

        return;
    }
    if (font){
        SDL_Rect dimensions2;
        double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
        double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
        dimensions2.x = box.x*scaleRatioW+cameraX*scaleRatioW+ ScreenManager::GetInstance().GetOffsetW();
        dimensions2.y = box.y*scaleRatioH+cameraY*scaleRatioH;
        dimensions2.h = box.h*scaleRatioH*scaleY;
        dimensions2.w = box.w*scaleRatioW*scaleY;

        if (!texture){
            RemakeTexture();
        }
        if (texture){
            SDL_SetTextureAlphaMod(texture,alpha);
            if (SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,NULL,&dimensions2,static_cast<double>(angle),nullptr,SDL_FLIP_NONE)){

                RemakeTexture();
            }
        }
    }else{
        Point p = texturespr->Render(text,box.x+cameraX,box.y+cameraY,alpha);
        box.w = p.x;
        box.h = p.y;
    }
}
void Text::RenderRS(int cameraX,int cameraY){
    if (!isWorking)
        return;
    if (font){
        SDL_Rect dimensions2;
        dimensions2.x = box.x+cameraX;
        dimensions2.y = box.y+cameraY;
        dimensions2.h = box.h;
        dimensions2.w = box.w;

        if (!texture){
            RemakeTexture();
        }
        if (texture){
            SDL_SetTextureAlphaMod(texture,alpha);
            if (SDL_RenderCopyEx(BearEngine->GetRenderer(),texture,NULL,&dimensions2,static_cast<double>(angle),nullptr,SDL_FLIP_NONE)){
                RemakeTexture();
            }
        }
    }else{
        Point p = texturespr->Render(text,box.x+cameraX,box.y+cameraY,alpha);
        box.w = p.x;
        box.h = p.y;
    }
}


void Text::SetText(std::string str){
    if (!font){
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

void Text::cpy(Text& t){
    text        = t.GetText();
    font        = t.font;
    texturespr  = t.texturespr;
    box         = t.box;
    isWorking   = t.isWorking;
    alpha       = t.alpha;
    angle       = t.angle;
    bg          = t.bg;
    size        = t.size;
    style       = t.style;
    color       = t.color;
    scaleY      = t.scaleY;
    scaleX      = t.scaleX;



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
    if (text == "")
        return;
    if (Game::GetInstance()->isClosing)
        return;
    if (!font && texturespr){
        Point p =  texturespr->GetSizes(text);
        box.w = p.x;
        box.h = p.y;
        return;
    }
    if (texture && Destory){
        SDL_DestroyTexture( texture );
    }

    texture = NULL;
    SDL_Surface *surf=NULL;
    if (style == TEXT_SOLID){
        surf = TTF_RenderText_Solid(font,text.c_str(),color);
    }else if(style == TEXT_SHADED){
        surf = TTF_RenderText_Shaded(font, text.c_str(),color, bg);
    }else if(style == TEXT_BLENDED){
        surf = TTF_RenderText_Blended(font,text.c_str(),color);
    }

    if (surf){
        texture = SDL_CreateTextureFromSurface(BearEngine->GetRenderer(),surf);
        Uint32 format;
        int acess,w,h;
        SDL_QueryTexture(texture, &format,&acess,&w,&h);
        box.h=h;
        box.w=w;
        SDL_Rect dimensions2;
        dimensions2.x = -box.w;
        dimensions2.y = -box.h;
        dimensions2.h = box.h;
        dimensions2.w = box.w;

        if (SDL_RenderCopy(BearEngine->GetRenderer(),texture,NULL,&dimensions2) < 0){
            std::cout << "[TXT:Render] Failed to render, SDL reason["<<SDL_GetError()<<"]:"<<"|"<<text.size()<<"{"<<text<<"}\n";
        }
        SDL_FreeSurface(surf);
    }else{
        std::cout << "[Text:RemakeTexture] Surface not loaded "<<SDL_GetError()<<"\n";
    }
}
