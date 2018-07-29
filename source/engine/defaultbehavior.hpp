#ifndef DEFBEHAVRIORH
#define DEFBEHAVRIORH
#include "behavior.hpp"
/**
    @brief Singleton class that will manage for you most of the behavior of the game
*/
class DefaultBehavior: public Behavior{
    public:
        /**
            @brief Singleton get
            *
            *
        */
        static DefaultBehavior& GetInstance();
        /**
            @brief Empty
            *
            *
        */
        DefaultBehavior();
        /**
            @brief Empty
            *
            *
        */
        ~DefaultBehavior();
        /**
            @brief Empty
            *
            *
        */
        bool OnLoad();
        /**
            @brief Clear assets saved in asset tables
            *
            *
        */
        void OnClose();
        /**
            @brief On resize, the game will request what to do to the class ConfigManager
            *
            * It may just increase the field of view of the camera\n
            * Simply deny the resize, and set to the original size\n
            * Scale free\n
            * Scale with restrictions\n
            *<b>Its important to add this to the GameBehavior::OnResize</b>
            @code
            bool GameBehavior::OnResize(int newW,int newH){
                return DefaultBehavior::GetInstance().OnResize(newW,newH); //This!!!!
            }
            @endcode

        */
        bool OnResize(int newW,int newH);
        /**
            @brief When lose/win focus
            @param isFocused true when you get the focus
        */
        void OnFocus(bool isFocused);

        void OnUpdate(float dt);

        void OnPreRender(){};
        /**
            @brief Empty Can be used on android to know if the game came to background or left background
        */
        void OnRestored();
        /**
            @brief Empty Can be used on android to know if the game came to background or left background
        */
        void OnMinimized();

        virtual bool Begin();

        virtual void DropFile(const char *c){};

        virtual bool Begin();
};
#endif // DEFBEHAVRIORH
