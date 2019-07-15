#pragma once
#include "basetypes.hpp"
#include <map>
#include <stack>
#include <functional>

class Sprite;
class AnimatedSprite;
class ScriptLoader;
class AnimationScript{
    public:
        AnimationScript():currentInstruction(nullptr),m_BaseAnimationTime(0.001f),m_LocalAnimationTime(0.0f),
        m_FrameCounter(0),m_LineCounter(0),m_MaxFrames(8),m_symbolCounter(0),m_innerCounter(0),m_instructionsOnFrame(0),m_spriteGrid(16,16),m_internalTimer(0.0f),m_currentDt(0.0f),
        m_justSwitchedCode(false),m_canUpdateFrames(false),m_animationOver(false),m_yieldToTop(false),m_lastCallbackValue(false),m_justSwitchedContext(false),m_persistenSettings(false){};

        bool Run(std::string Label);

        void Update(float dt);

        void Kill();

        uint32_t QuerySymbol(std::string& symb);

        void AddInstructionSet(uint32_t id,InstructionSet &instructions);

        bool CanUpdate() const{
            return m_canUpdateFrames;
        }

        bool IsRunning();
        void UpdateSprite(Sprite &sp);
        void UpdateSprite(AnimatedSprite &sp);


        void FormatSprite(AnimatedSprite &sp);

        void AddCallback(std::string labelName,std::function<bool()> &cb);



    private:
        friend class ScriptLoader;


        AnimInstruction *currentInstruction;

        typedef struct{
            uint32_t programCounter;
            uint32_t sequenceRunner;
        } executionContext;


        void RunInstruction(executionContext &ec);
        bool ParseInstruction();
        void AddContext();

        void ParseRange(bool &terminate, bool &nextInstruction);
        void ParseTime(bool &terminate, bool &nextInstruction);
        void ParseJump(bool &terminate, bool &nextInstruction);
        void ParseLine(bool &terminate, bool &nextInstruction);
        void ParseLoop(bool &terminate, bool &nextInstruction);
        void ParseRepeat(bool &terminate, bool &nextInstruction);
        void ParseAuxiliar(bool &terminate, bool &nextInstruction);
        void ParseWait(bool &terminate, bool &nextInstruction);
        void ParseSingleFrame(bool &terminate, bool &nextInstruction);
        void ParseCallback(bool &terminate, bool &nextInstruction);
        void ParseConditionJump(bool &terminate, bool &nextInstruction);


        void ResetLocalValues();


        float m_BaseAnimationTime;
        float m_LocalAnimationTime;
        uint32_t m_ProgramCounter;
        uint32_t m_SequenceRunner;
        uint32_t m_FrameCounter;
        uint32_t m_LineCounter;
        uint32_t m_MaxFrames;
        uint32_t m_symbolCounter;
        uint32_t m_innerCounter;
        uint32_t m_instructionsOnFrame;
        PointInt m_spriteGrid;

        float m_internalTimer, m_currentDt;

        bool m_justSwitchedCode,m_canUpdateFrames,m_animationOver, m_yieldToTop, m_lastCallbackValue,m_justSwitchedContext,m_persistenSettings;


        uint32_t AddSymbol(std::string&& symb);

        std::map<std::string,uint32_t> m_symbolSTI;
        std::map<uint32_t,std::string> m_symbolITS;

        std::map<uint32_t,InstructionSet> m_instructions;

        std::map<uint32_t,std::function<bool()> > m_callbacks;

        std::stack<executionContext> m_context;



};
