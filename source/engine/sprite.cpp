#include "sprite.hpp"
#include "gamebase.hpp"
#include <math.h>
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "renderhelp.hpp"
#include "camera.hpp"
#include "smarttexture.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/debughelper.hpp"
#include "../image/stb_image.h"

TextureLoadMethod TextureLoadMethod::DefaultLoadingMethod = TEXTURE_NEAREST;

ColorReplacer::ColorReplacer(){

}
void ColorReplacer::AddReplacer(uint32_t from,uint32_t to){
    canReplace[from] = true;
    Replace[from] = to;
}

uint32_t ColorReplacer::Get(uint32_t color){
    if (canReplace[color]){
        return Replace[color];
    }
    return color;
}


Sprite::Sprite(){
    m_lf = 0;
    dimensions.w = dimensions.h = dimensions.x = dimensions.y = 0;
    scaleX = scaleY = 1;
    currentFrame = PointInt(0,0);
    frameCount = 1;
    repeat = 1;
    hasCenter = false;
    aliasing = TEXTURE_LINEAR;
    m_alpha = 1.0f;
    fname = "";
    over = 0;
    OUTR = OUTB = OUTG = 1.0f;
    timeElapsed = 0;
    frameTime = 0;
    sprFlip = SDL_FLIP_NONE;

}

Sprite::Sprite(TexturePtr texture_,std::string name,std::string alias,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    textureShred = texture_;
    fname = alias;
    aliasing = hasAliasing;
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}


Sprite::Sprite(TexturePtr texture_,std::string name,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    textureShred = texture_;
    fname = name;
    aliasing = hasAliasing;
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,std::string alias,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    textureShred = texture_;
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = alias;
    aliasing = hasAliasing;
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    textureShred = texture_;
     frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    aliasing = hasAliasing;
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
}

Sprite::Sprite(TexturePtr texture_,std::string name,ColorReplacer &r,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode= TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    aliasing = hasAliasing;
    textureShred = texture_;
    Query(textureShred);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,ColorReplacer &r,bool replacer,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = file;
    aliasing = hasAliasing;
    scaleX = scaleY = 1;
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,file,r,hasAliasing);
    if (textureShred.get()){
        Query(textureShred);
    }
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);

}


Sprite::Sprite(SDL_RWops* file,std::string name,int fcount,float ftime,int rep,TextureLoadMethod hasAliasingg):Sprite(){
    if (hasAliasingg.mode == TEXTURE_DEFAULT){
        hasAliasingg.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    repeat = rep;
    frameTime = ftime;
    fname = name;
    Open(file,name,hasAliasingg);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    fname = file;
    Open(file,hasAliasing);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

Sprite::Sprite(std::string file,int fcount,float ftime,int rep,TextureLoadMethod hasAliasing):Sprite(){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    frameCount = fcount;
    fname = file;
    frameTime = ftime;
    repeat = rep;
    Open(file,hasAliasing);
    frameCount = std::max(1,frameCount);
    SetGrid(GetWidth()/frameCount,GetHeight());
    SetFrame(0);
    SetAlpha(255);
}

void Sprite::Kill(){
    //textureShred.destroy();
}

void Sprite::Update(float dt){
    timeElapsed = timeElapsed+dt;
    if (timeElapsed >= frameTime){
        timeElapsed = 0;
        currentFrame.x++;
        if (currentFrame.x >= frameCount){
            currentFrame.x = 0;
            over++;
        }
        SetFrame(currentFrame.x);
    }
    if (IsAnimationOver() && m_lf > 0){
        currentFrame.x = m_lf;
        SetFrame(currentFrame.x);
        return;
    }
}

void Sprite::SetGrid(int gx,int gy){
    gx = std::min(gx,GetWidth());
    grid.x = gx;
    gy = std::min(gy,GetHeight());
    grid.y = gy;
    if (gx != 0){
        SetFrameCount(GetWidth()/gx);
    }

}

void Sprite::SetFrame(int xFrame,int yFrame){
    if (yFrame != -1)
        currentFrame.y = yFrame;
    currentFrame.x = xFrame;
    if (currentFrame.x >= frameCount){
        currentFrame.x = 0;
    }
    SetClip(currentFrame.x*(grid.x), currentFrame.y * (grid.y) ,grid.x,grid.y);
}

Sprite::~Sprite(){}



BearTexture *Sprite::Preload(const char *file,bool adjustDir,TextureLoadMethod HasAliasing){
    if (HasAliasing.mode == TEXTURE_DEFAULT){
        HasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    char sprn[1024];
    strcpy(sprn,file);
    std::string stdnamee(sprn);

    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        SDL_RWops *rw = ResourceManager::GetInstance().GetFile(stdnamee); //Safe
        BearTexture *returnTexture = Sprite::Preload(rw,stdnamee,HasAliasing);
        SDL_RWclose(rw);
        return returnTexture;
    }
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);

    unsigned char* imageData = nullptr;
    SDL_RWops* rw = SDL_RWFromFile(stdnamee.c_str(), "rb");
    if (!rw){
        Console::GetInstance().AddTextInfo(utils::format("[Standard]Cannot preload sprite [%s] because: %s",stdnamee.c_str(),SDL_GetError()));
        return nullptr;
    }
    uint64_t rsize;
    int sizeX,sizeY,comp;
    char* res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    SDL_RWclose(rw);
    if (imageData){
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        HasAliasing.ApplyFilter();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(imageData);
        BearTexture *ret = new BearTexture(texId,sizeX,sizeY,GL_RGBA);
        ret->textureMode = HasAliasing;
        return ret;
    }
    return nullptr;
}

BearTexture * Sprite::Preload(SDL_RWops* rw,std::string name,TextureLoadMethod hasAliasing){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    uint64_t rsize;
    if (rw == nullptr){
        Console::GetInstance().AddTextInfo(utils::format("Cannot preload rw sprite [%s]",name.c_str()));
        return nullptr;
    }
    char *res = ResourceManager::GetFileBuffer(rw,rsize);
    imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
    ResourceManager::ClearFileBuffer(res);
    if (imageData){
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        hasAliasing.ApplyFilter();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(imageData);
        BearTexture *ret = new BearTexture(texId,sizeX,sizeY,GL_RGBA);
        ret->textureMode = hasAliasing;
        return ret;
    }


    return nullptr;
}
BearTexture * Sprite::Preload(std::string fileName,ColorReplacer &r,TextureLoadMethod hasAliasing){

    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    if (fileName == ""){
        return nullptr;
    }
    unsigned char* imageData = nullptr;
    int sizeX,sizeY,comp;
    std::string aux = fileName;
    if (ResourceManager::IsValidResource(aux)){
        /**
            Loading from rwops
        */
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(fileName); //safe
        if (!rw){
            bear::out << "Error loading ["<<fileName<<"]: " << SDL_GetError() << "\n";
            return nullptr;
        }
        uint64_t rsize;
        char *res = ResourceManager::GetFileBuffer(rw,rsize);
        imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
        ResourceManager::ClearFileBuffer(res);
        SDL_RWclose(rw);
    }else{
        GameFile tempFile;
        if (tempFile.Open(fileName,true)){
            tempFile.Cache();
            char *res = tempFile.GetCache_Unsafe();
            uint64_t rsize = tempFile.GetSize();
            imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
            tempFile.Close();
        }
    }
    if (imageData){

        /*
            Pixel replacing
        */

        int pixelCount = (sizeX*sizeY);
        for( int i = 0; i < pixelCount; ++i ){
            ((Uint32*)imageData)[i] = r.Get(((Uint32*)imageData)[i]);
        }

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        hasAliasing.ApplyFilter();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(imageData);
        BearTexture *ret = new BearTexture(texId,sizeX,sizeY,GL_RGBA);
        ret->textureMode = hasAliasing;
        return ret;
    }
    return nullptr;
}

void Sprite::Query(TexturePtr ptr){
    BearTexture *texturee = ptr.get();
    if (texturee != NULL){
        dimensions.w = texturee->w;
        dimensions.h = texturee->h;
        SetClip(0,0,dimensions.w,dimensions.h);
    }
}

bool Sprite::Open(std::string filepath,TextureLoadMethod hasAliasing){
    if (hasAliasing.mode == TEXTURE_DEFAULT){
        hasAliasing.mode = TextureLoadMethod::DefaultLoadingMethod.mode;
    }
    scaleX = scaleY = 1;
    std::string stdnamee(filepath);
    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        SDL_RWops* file = ResourceManager::GetInstance().GetFile(stdnamee); //safe
        bool ret = Open(file,stdnamee,hasAliasing);
        SDL_RWclose(file);
        return ret;
    }
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,stdnamee,hasAliasing);
    if (textureShred.get()){
        Query(textureShred);
        return true;
    }
    return false;
}

bool Sprite::Open(SDL_RWops* file,std::string name,TextureLoadMethod HasAliasing){
    scaleX = scaleY = 1;
    textureShred = GlobalAssetManager::GetInstance().makeTexture(false,file,name,HasAliasing);
    if (textureShred.get()){
        Query(textureShred);
        return true;
    }else{
        #ifdef __EMSCRIPTEN__
        std::string newName = name;
        char *c = (char*)newName.c_str();
        for (int i=0;i<newName.size();i++){
            if (c[i] == ':'){
                c[i] = '/';
                break;
            }
        }
        Console::GetInstance().AddText(utils::format("Loading of the sprite [%s] cannot be loaded using resource tweaks at emscripten",name.c_str()));
        Console::GetInstance().AddText(utils::format("Trying instead to load [%s]",newName.c_str()));
        return Open((char*)newName.c_str());
        #else
        Console::GetInstance().AddText(utils::format("Cannot load rwop sprite FAAAAIL [%s]",name.c_str()));
        return false;
        #endif // __EMSCRIPTEN__
    }
}

void Sprite::SetClip(int x, int y,int w,int h){
    clipRect.x = x;
    clipRect.y = y;
    clipRect.h = h;
    clipRect.w = w;

}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sprite::Render(PointInt pos,double angle){
    if (IsLoaded()){
        #ifndef RENDER_OPENGL
        double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
        double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
        dimensions2.x = pos.x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
        dimensions2.y = pos.y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
        dimensions2.h = clipRect.h*scaleRatioH*scaleY;
        dimensions2.w = clipRect.w*scaleRatioW*scaleX;
        SDL_RenderCopyEx(BearEngine->GetRenderer(),textureShred.get(),&clipRect,&dimensions2,(angle),hasCenter ? &center : NULL,sprFlip);
        #else



        static unsigned int VBOEE;
        static GLuint VBO;
        static bool made = false;
        static Shader baseShader;

        float w = dimensions.w;
        float h = dimensions.h;

        texLeft = clipRect.x / (float)w; //0
        texRight =  ( clipRect.x + clipRect.w ) / (float)w; //1
        texTop = clipRect.y / (float)h; //0
        texBottom = ( clipRect.y + clipRect.h ) / (float)h; //1


        GLfloat vertices[] = {
                // Pos      // Tex
            0.0f, 1.0f, texLeft, texBottom,
            1.0f, 0.0f, texRight, texTop,
            0.0f, 0.0f, texLeft, texTop,

            0.0f, 1.0f, texLeft, texBottom,
            1.0f, 1.0f, texRight, texBottom,
            1.0f, 0.0f, texRight, texTop
        };


        if (!made){


            // Configure VAO/VBO



            glGenVertexArrays(1, &VBOEE);
            glGenBuffers(1, &VBO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindVertexArray(VBOEE);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            made = true;
            baseShader.Compile(GL_VERTEX_SHADER,"sprvertex.glvs");
            baseShader.Compile(GL_FRAGMENT_SHADER,"sprfrag.glfs");
            baseShader.Link();
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);







        baseShader.Bind();

        glm::vec2 size(clipRect.w,clipRect.h);
        glm::mat4 model(1.0f);

        model = glm::translate(model, glm::vec3((float)pos.x,(float)pos.y, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
        model = glm::rotate(model, float(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size.x * scaleX,size.y * scaleY, 1.0f));

        glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_SIZE_W,  (float)SCREEN_SIZE_H, 0.0f, -1.0f, 1.0f);



        unsigned int transformLoc = glGetUniformLocation(baseShader.GetCurrentShaderId(), "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

        transformLoc = glGetUniformLocation(baseShader.GetCurrentShaderId(), "projection");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));

        baseShader.SetUniform<Point3>("spriteColor",Point3(OUTR, OUTG, OUTB));
        baseShader.SetUniform<int>("image",0);




        glActiveTexture(GL_TEXTURE0);

        glBindTexture( GL_TEXTURE_2D, textureShred.get()->id );
        glBindVertexArray(VBOEE);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        baseShader.Unbind();

        /*
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glColor4f(OUTR, OUTG, OUTB, m_alpha);
        float w = dimensions.w;
        float h = dimensions.h;

        texLeft = clipRect.x / (float)w;
        texRight =  ( clipRect.x + clipRect.w ) / (float)w;
        texTop = clipRect.y / (float)h;
        texBottom = ( clipRect.y + clipRect.h ) / (float)h;

        GLfloat quadWidth = clipRect.w ;
        GLfloat quadHeight = clipRect.h ;

        glScalef(scaleX , scaleY , 1.0f);

        glTranslatef(
            (pos.x * (1.0f/scaleX)  + quadWidth  / 2.f  ) + (- center.x* (scaleX)  + center.x),
            (pos.y * (1.0f/scaleY)  + quadHeight / 2.f  ) + (- center.y* (scaleY)  + center.y),
        0.f);

        glRotatef( angle, 0.f, 0.f, 1.f );

        glBindTexture( GL_TEXTURE_2D, textureShred.get()->id );
        if ((sprFlip&SDL_FLIP_HORIZONTAL) != 0){
            float holder =  texLeft;
            texLeft = texRight;
            texRight = holder;
        }
        if ((sprFlip&SDL_FLIP_VERTICAL) != 0){
            float holder =  texTop;
            texTop = texBottom;
            texBottom = holder;
        }

        glBegin( GL_TRIANGLE_FAN );
            glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
            glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
            glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
            glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
        glEnd();
        */


        #endif // RENDER_OPENGL
    }

}
void Sprite::Renderxy(int x,int y,double angle){
    Render(x-Camera::pos.x,y-Camera::pos.y,angle);
}
void Sprite::Render(int x,int y,double angle){
    Render(PointInt(x,y),angle);
}


void  Sprite::Format(Animation anim,int dir){
    if (IsLoaded()){
            return;
    }
    if (dir == 0){
        SetFlip(SDL_FLIP_NONE);
    }else if (dir == 1){
        SetFlip(SDL_FLIP_HORIZONTAL);
    }else if (dir == 2){
        SetFlip(SDL_FLIP_VERTICAL);
    }
    SetClip(anim.sprX * anim.sprW, anim.sprY * anim.sprH,anim.sprW,anim.sprH);
    anim.isFormated = true;
}

int Sprite::GetWidth(){
    return dimensions.w*scaleX;
}

int Sprite::GetHeight(){
    return dimensions.h*scaleY;
}
int Sprite::GetFrameHeight(){
    return clipRect.h*scaleY;
}

int Sprite::GetFrameWidth(){
    return clipRect.w*scaleX;
}

