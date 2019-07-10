#pragma once
#include "basetypes.hpp"
#include <map>
#include <stack>
#include <stdio.h>


class AnimationLoader{
    public:
        AnimationLoader():currentInstruction(nullptr),m_BaseAnimationTime(0.0f),m_LocalAnimationTime(0.0f),
        m_FrameCounter(0),m_LineCounter(0),m_MaxFrames(8),m_symbolCounter(0),m_internalTimer(0.0f),m_currentDt(0.0f),
        m_justSwitchedCode(false),m_canUpdateFrames(false),m_animationOver(false){};

        bool LoadFile(std::string path);

        void Run(std::string Label);

        void Update(float dt);

        uint32_t QuerySymbol(std::string& symb);

        void AddInstructionSet(uint32_t id,InstructionSet &instructions);

    private:
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


        void ResetLocalValues();


        float m_BaseAnimationTime;
        float m_LocalAnimationTime;
        uint32_t m_ProgramCounter;
        uint32_t m_SequenceRunner;
        uint32_t m_FrameCounter;
        uint32_t m_LineCounter;
        uint32_t m_MaxFrames;
        uint32_t m_symbolCounter;

        float m_internalTimer, m_currentDt;

        bool m_justSwitchedCode,m_canUpdateFrames,m_animationOver;





        uint32_t AddSymbol(std::string&& symb);

        std::map<std::string,uint32_t> m_symbolSTI;
        std::map<uint32_t,std::string> m_symbolITS;

        std::map<uint32_t,InstructionSet> m_instructions;

        std::stack<executionContext> m_context;



};
