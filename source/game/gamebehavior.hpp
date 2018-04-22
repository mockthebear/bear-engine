#ifndef GAMEBEHAVIORH
#define GAMEBEHAVIORH
#include "../engine/defaultbehavior.hpp"
/**
    @brief The mandatory class inside the folder game. It will be there to notify you about some things
    *
    *This is the only mandatory class in your game.
    *Check DefaultBehavior and Behavior
    *
    *<b>All these functions call right the way their cousin DefaultBehavior. Please don't remove unless you know
    *what are you doing</b>
*/
class GameBehavior: public DefaultBehavior{
    public:
        /**
            @brief Singleton get
            *
            *
        */
        static GameBehavior& GetInstance();
        /**
            @brief This will be created before the SDL even starts
        */
        GameBehavior();
        /**
            @brief At the game close
        */
        ~GameBehavior();
        /**
            @brief Here you should add your game state, otherwise the game will close
            *
            @code
            Game::GetInstance()->AddState(new YourState());
            @endcode
        */
        bool OnLoad();
        bool Begin();
        /**
            Delete some global thing that you may have created. Or even save data
        */
        void OnClose();
        /**
            Resize notificatin and action
        */
        bool OnResize(int newW,int newH);
        /**
            @brief When lose/win focus
            @param isFocused true when you get the focus
        */
        void OnFocus(bool isFocused);
        /**
            @brief Empty Can be used on android to know if the game came to background or left background
        */
        void OnRestored();
        /**
            @brief Empty Can be used on android to know if the game came to background or left background
        */
        void OnMinimized();


};
#endif // DEFBEHAVRIORH

