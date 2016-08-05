#ifndef BE_CONSOLE
#define BE_CONSOLE
#include "../engine/text.hpp"
#include "../framework/utils.hpp"
#include "../framework/userfile.hpp"
#include <fstream>


#define AddTextInfo(str) AddText("[%s] %s",utils::methodName(__PRETTY_FUNCTION__),std::string(str).c_str())
#define AddTextInfoF(str,...) AddTextInfo(utils::format(std::string(str).c_str(),__VA_ARGS__))
#define AddTextInfoLine(str) AddText("[%s %d] %s",utils::methodName(__PRETTY_FUNCTION__),__LINE__,std::string(str).c_str())
#define AddTextInfoLineF(str,...) AddTextInfoLine(utils::format(std::string(str).c_str(),__VA_ARGS__))

#ifndef DISABLE_LUAINTERFACE
#define LuaWarningMessage(str) Console::GetInstance().AddText(utils::format("[Lua:Warning] %s"),str)
#define LuaWarningMessageF(str,...) LuaWarningMessage(utils::format(str,__VA_ARGS__));
#endif


/**
    @brief Console plus logging
    *
    * This class may be used to log some output, also print it on screen.
    *
    * Based on class Text to render things on screen.
*/
class Console{
    public:
        /**
            @brief Constructor
            *
            * The class actually have a singleton, but you can create one for you own use
            *
            * When the file is enabled, it will use the file until the class is deleted
            *
            @param lines the amount of lines
            @param width the maximum width of the black square
            @param file to the class know if the data will be saved on an file. This is the name
            leave empty to dont save in any file.
        */
        Console(int lines, int width,std::string file="");
        /**
            The console when not started with Console::Begin will not show any text
        */
        void Begin();
        /**
            Here is where the file is closed.
        */
        ~Console();

        /**
            Close Output
        */
        void CloseOutput();
        /**
            The console when not started with Console::Begin will not show any text
        */
        static Console& GetInstance(bool writeFile=false);
        /**
            Render the console on X Y at screen
        */
        void Render(Point p=Point(0,0));
        /**
            Add an raw text line to the console
        */
        bool AddText(std::string str);
        /**
            Will be replaced by an macro
        */
        /*bool AddTextInfo(std::string str){return true;};
        template<typename... arg> bool AddTextInfoF(std::string str,const arg&... a){return true;};
        */
        /**
            Will be replaced by an macro
        */
        //bool AddTextInfoLine(std::string str){return true;};
        //template<typename... arg> bool AddTextInfoLineF(std::string str,const arg&... a){return true;};

        /**
            Add an empty line
        */
        bool AddText(){
            return AddText("");
        }
        /**
            *Template to format things.
            *
            *void Console::AddTextInfo(std::string str)
            *Print like:
            *[Class:Method] Text
            *
            *void Console::AddTextInfoLine(std::string str)
            *Print like:
            *[Class:Method 25(line)] Text
        */
        template<typename... arg> bool AddText(std::string str, const arg&... a){
            std::string str2 = utils::format(str,a...);
            return AddText(str2);
        }
        /* Got this from here:

            http://stackoverflow.com/questions/1666802/is-there-a-class-macro-in-c

            with some modifications

        */


    private:
        bool logFile,failFile;
        UserFile myfile;
        bool hasStarted;
        Rect box;
        int counter;
        int m_lines;
        Text **m_line;

};


#endif // BE_CONSOLE
