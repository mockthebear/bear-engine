// *** ADDED BY HEADER FIXUP ***
#include <iterator>
// *** END ***
#include "../framework/geometry.hpp"
#include "sprite.hpp"


#include "../settings/definitions.hpp"
#include SDL_TTF_LIB_HEADER
#include <unordered_map>
#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#ifndef TEXTH
#define TEXTH

enum TextStyle{
    TEXT_SOLID,
    TEXT_SHADED,
    TEXT_BLENDED
};

enum TextRenderStyle{
    TEXT_RENDER_TOPLEFT     = 0b0000,
    TEXT_RENDER_TOPRIGHT    = 0b0001,
    TEXT_RENDER_BOTTOMLEFT  = 0b0010,
    TEXT_RENDER_BOTTOMRIGHT = 0b0011,
    TEXT_RENDER_CENTERHOR   = 0b0100,
    TEXT_RENDER_CENTERVER   = 0b1000,
    TEXT_RENDER_CENTER      = 0b1100,
};

/**
 * @brief Class used to save some information about an letter in the class CustomFont
 *
 * This class saves only data
 * Check CustomFont. Seriously
 */
class Letter{
    public:
        Letter():x(0),y(0),w(0),h(0),padx(0),pady(0),xoffset(0),yoffset(0),resetX(false),valid(false){}
        Letter(int xx,int yy,int ww, int hh, int ppadX,int ppadY,int ppadin,int yffseta,bool resetXa):Letter(){
            valid = true;
            x = xx;
            y = yy;
            h = hh;
            w = ww;
            padx = ppadX;
            pady = ppadY;
            xoffset = ppadin;
            yoffset = yffseta;
            this->resetX = resetXa;
        }

        int x,y,w,h,padx,pady,xoffset,yoffset;
        bool resetX,valid;
};
/**
 * @brief Custom frons from images
 *
 * Some times, using the default ttf lib its not enough. The text keep showing pixels
 * and some times, its hard to understand what is. So it better make a font by yourself
 * then, render clipped images to work as a font. This class does it for you
 * Lets say you have an image like this:
 \image html images/numbers.png
 *Then the custom font config file will be named "nums.burrt"
 *Inside will be:
 @code
#Structure:
# Letter = <x,y,w,h,pading in, xoffset,yoffset>
# Paddingin: The distance to the next letter.  Render(x,y); x += w + padding in;
# xoffset out: Offset on x  Render(x+xoffset,y);
# yoffset out: Offset on y  Render(x,y+yoffset);
#
@<img/interface/numeros.png>:
    0=<3,3,27,21,0,0,0>;
    1=<30,3,27,21,0,0,0>;
    2=<57,3,27,21,0,0,0>;
    3=<84,3,27,21,0,0,0>;
    4=<111,3,27,21,0,0,0>;
    5=<138,3,27,21,0,0,0>;
    6=<165,3,27,21,0,0,0>;
    7=<192,3,27,21,0,0,0>;
    8=<219,3,27,21,0,0,0>;
    9=<246,3,27,21,0,0,0>;
@
 @endcode
 *the meaning is explained at the comments
 *You basically uses:
@code
@<imagePath>:
LETTER=<X,Y,W,H,PADDING IN,XOFFSET,YOFFSET>;
@
@endcode
@ <b>Bythe way, the class Text manages this class for you.</b>
 */
class CustomFont{
    public:
        /**
            *Constructor
        */
        CustomFont():sp(),loaded(false),oldAlpha(255),Letters(){};
        CustomFont(std::string letterpositions);
        /**
            *BYE
        */
        ~CustomFont();
        /**
            *This will render the text at screen, and return the value of sizes
            @return Point containing the width and height of the text
        */
        Point Render(const char*c,int x,int y,int alpha=255){
            return Render(std::string(c),x,y,alpha);
        };
        Point Render(std::string str,int x,int y,int alpha=255);
        Point RenderCentered(std::string c,PointInt p,int alpha=255){
            Point sz = GetSizes(c);
            return Render(c,p.x-sz.x/2.0f,p.y,alpha);
        }
        Point Renderl(std::string c,PointInt p,int alpha=255){
            return Render(std::string(c),p.x,p.y,alpha);
        }
        /**
            @return Point containing the width and height of the text
        */
        Point GetSizes(std::string str);
        void SetSprite(Sprite spr){sp = spr;};
        Sprite sp;
        bool loaded;
    private:


        int oldAlpha;
        std::map<unsigned char, Letter> Letters;
};


/**
 * @brief Base class to manage Texts
 *
 * In this class you can manage texts.
 * Using fonts like .ttf or custom fonts
 * When a font is loaded, it keeps saved always on Text::assetTable
 * Like the class Sprite does on Sprite::assetTable
 */

class Text{
    public:

        /**
            *Create an text from an .ttf file or .otf
            @param fontfile the path for the font
            @param fontsize the size that will be used
            @param style means the style. Check the enum TextStyle (TEXT_SOLID is the base one)
            @param text here you can put silly texts
            @param color the color in RGB
            @param x When you want to set an offset to the text. Same to y. Default is 0
        */
        Text(std::string fontfile, int fontsize,TextStyle style, std::string text, SDL_Color color,int x=0,int y=0);
        /**
            *The engine uses an default font. "engine:default.ttf"
            *This is the simplest version of any text.
            *Use this creator for some fast text opening
            *Accept resource tweaks
            @param text the silly text to be displayed
            @param size you need a size... or not. default is 12
            @param color the default is gray r100,g100,b100
        */
        Text(std::string text,int size=12,SDL_Color color={100,100,100,255});
        /**
            *The custom font thing... You should check the class CustomFont to understand
            *how custom fonts work
            *Accept resource tweaks
            *
            *This creator simply get an path to a custom font, then uses it on a given text
        */

        Text(std::string customfontfile, std::string text,int x=0,int y=0);
        /**
            *Empty constructor is almost empty
        */
        Text():box(),bg({100,100,120,255}),angle(0),fontfile(""),text(""),style(TEXT_SOLID),size(),scaleY(1.0f),scaleX(1.0f),
        color({255,255,255,255}),alpha(255),font(nullptr),isWorking(false),texture(nullptr),aliasing(TEXTURE_LINEAR),texturespr(nullptr),keepAlive(false){};

        void SetKeepAlive(bool set){
            keepAlive = set;
        };
        /**
            *Here it just delete the texture.
        */

        virtual ~Text();
        /**
            *Close everything from the text manually for future reuse
        */
        void Close();
        /**
            *Here you render the text at a given position
            *This possition is added to the offset on constructor
        */
        void Render(int X = 0, int Y = 0,TextRenderStyle renderStyle = TEXT_RENDER_TOPLEFT);
        void RenderLua(Point p){
            Render(p.x,p.y,TEXT_RENDER_TOPLEFT);
        }
        /**
            Render without any scaling or screen adjustment
        */
        void inline Render(PointInt p,TextRenderStyle renderStyle = TEXT_RENDER_TOPLEFT){Render(p.x,p.y,renderStyle);};
        /**
            *When you edit the current text, a new texture is created
            *Unless you're using custom fonts, then just an std::string will be changed
            @param str new text
        */
        void SetText(std::string str);
        /**
            *When you edit the current text, a new texture is created
            *This dont work with custom fonts
            @param color in rgb
        */
        void SetColor(SDL_Color color);
        /**
            *When you edit the current text, a new texture is created
            *This dont work with custom fonts
            @param color in rgb
        */
        void SetStyle(TextStyle style);
        /**
            *When you edit the current text, a new texture is created
            *Unless you're using custom fonts
            @param style in TextStyle
        */
        void SetAlpha(int alpha);
        /**
            *Trivial...
        */
        std::string GetText(){return text;};
        /**
            *Get the height of the text
            @return int
        */
        float GetHeight(){return box.h*scaleY;};
        /**
            *Get the width of the text
            @return int
        */
        float GetWidth(){return box.w*scaleX;};
        /**
            *When you edit the current text, a new texture is created
            *Dont work with custom fonts
        */
        void SetFontSize(int size);

        void SetFont(std::string stra){
            InternalSetFont(stra);
            RemakeTexture();
        }

        void InternalSetFont(std::string fnt);
        #ifndef RENDER_OPENGL
        void SetAliasign(bool al){
        #else
        void SetAliasign(TextureLoadMethod al){
        #endif // RENDER_OPENGL
            aliasing = al;
            RemakeTexture();

        }



        /**
            *Clear assetTable
        */
        static void Clear();
        /**
            *You can scale your text. This is useful because you can start with a text size 30
            *then you set the scale to 0,2. The scaling from SDL2 is better than scaling on freetype
            @param scale the current scale is 1.0f
        */
        void SetScaleX(float scale){scaleX=scale;};
        /**
            *You can scale your text. This is useful because you can start with a text size 30
            *then you set the scale to 0,2. The scaling from SDL2 is better than scaling on freetype
            @param scale the current scale is 1.0f
        */
        void SetScaleY(float scale){scaleY=scale;};
        /**
            *When for any reason need to remake the texture. Usualy is called by the class every time you change
            *the text, color, alpha...
        */
        void RemakeTexture(bool destroy = true);
        /**
            *Its always good check if the text is working
        */
        bool IsWorking(){return isWorking;};
        /**
            *Rotate the
            *T
            *e
            *x
            *t
            *!
            *
            @param anglee is in degrees
        */
        void SetRotation(float anglee){angle = anglee;};


        /**
            *Now this operator is tricky.
            *In sprite we dont have this issue, because the texture is saved on
            *a table. In text we dont have this, we save only the font. Its an problem, specially when you
            *have an crucial thing saved on pointer. The destructor, when set to destroy this pointer have some issues in static
            *cases. What i have made to solve this? when you copy an static or temporary member, it copy everything, but copy the
            *texture to the new instance and erase the current texture.
            *erasing the current pointer its not really an problem. When you call render, and in the render is seen an null texture
            *it tries to remake the texture.
            *
            *CRYSIS AVOIDED
        */
        /*Text& operator=(Text T){
            cpy(&T);
            texture = nullptr;
            RemakeTexture();
            return *this;
        }*/
        Text& operator=(Text const &T){
            cpy((Text*)(&T));
            texture = nullptr;
            RemakeTexture();
            return *this;
        }




    private:
        Rect box;
        SDL_Color bg;
        float angle;

        void cpy(Text *t);
        static std::unordered_map<std::string, TTF_Font*> assetTable;
        static std::unordered_map<std::string, CustomFont*> customTable;

        std::string fontfile,text;
        TextStyle style;
        int size;
        float scaleY,scaleX;
        SDL_Color color;

        int alpha;

        TTF_Font* font;
        bool isWorking;
        #ifndef RENDER_OPENGL
        SDL_Texture* texture;
        bool aliasing;
        #else
        std::shared_ptr<BearTexture> texture;
        TextureLoadMethod aliasing;
        #endif // RENDER_OPENGL
        CustomFont *texturespr;
        bool keepAlive;





};

#endif
