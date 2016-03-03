#ifndef BEHAVIORH
#define BEHAVIORH

/**
    @brief Virtual class to manage game activity not related to gameplay. Like DefaultBehavior
    *
    *On class DefaultBehavior you will have an singleton of the main engine activity and
    *description of what the engine will do at each function.
*/
class Behavior{
    public:
        Behavior(){};
        ~Behavior(){};
        /**
            @brief Function called when the game is opening
            *
            * Should be used to create and add the GenericState
            @code
            Game::GetInstance().AddState(new YourState());
            @endcode
        */
        virtual bool OnLoad()=0;
        /**
            @brief Function called when the game is opening
            *
            * Here you can delete some global stuff
        */
        virtual void OnClose()=0;
        /**
            @brief You wiill get notified if the screen has changed in size
        */
        virtual bool OnResize(int newW,int newH)=0;
        /**
            @brief When lose/win focus
            @param isFocused true when you get the focus
        */
        virtual void OnFocus(bool isFocused)=0;
        /**
            @brief Can be used on android when you resume the app
        */
        virtual void OnRestored()=0;
        /**
            @brief Can be used on android when you resume the app
        */
        virtual void OnMinimized()=0;
};
#endif // BEHAVIORH
