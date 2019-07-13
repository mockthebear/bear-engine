#include "animationscript.hpp"
#include "animation.hpp"

#include "../../framework/utils.hpp"
#include "../sprite.hpp"
#include <iostream>


void AnimationScript::UpdateSprite(Sprite &sp){
    sp.SetClip(m_spriteGrid.x * m_FrameCounter, m_spriteGrid.y * m_LineCounter, m_spriteGrid.x, m_spriteGrid.y);
}

void AnimationScript::UpdateSprite(AnimatedSprite &sp){

        sp.sprW = m_spriteGrid.x;
        sp.sprH = m_spriteGrid.y;

    sp.SetClip(sp.sprW * m_FrameCounter, sp.sprH * m_LineCounter, sp.sprW, sp.sprH);
}


bool AnimationScript::Run(std::string Label){
    if (m_symbolSTI.find(Label) != m_symbolSTI.end()){
        executionContext ec;
        ec.programCounter = 0;
        ec.sequenceRunner = m_symbolSTI[Label];
        std::cout << "Added sequence " << ec.sequenceRunner << " : "<<Label<<"\n";
        if (m_context.size() > 0)
            m_context.pop();
        currentInstruction = nullptr;
        m_context.push(ec);
        return true;
    }
    return false;
}

void AnimationScript::RunInstruction(executionContext &ec){
    //std::cout << "Instruction on:  " << m_symbolITS[ec.sequenceRunner] << "\n";

    m_justSwitchedCode = false;
    m_animationOver = false;
    m_canUpdateFrames = false;


    if (m_yieldToTop){
        m_yieldToTop = false;
        ResetLocalValues();
        ec.programCounter--;
        currentInstruction = nullptr;
       // std::cout << "Bigger context asked to yeld to "<<m_symbolITS[m_context.top().sequenceRunner]<<".\n";
        RunInstruction(m_context.top());
        return;
    }
    if (currentInstruction == nullptr){
        std::map<uint32_t,InstructionSet>::iterator it = m_instructions.find(ec.sequenceRunner);
        if (it != m_instructions.end()){
            if (it->second.size() > ec.programCounter){
                currentInstruction = &it->second[ec.programCounter];
                ++ec.programCounter;
                m_justSwitchedCode = true;
                //std::cout << "Increased PC to "<< ec.programCounter << "\n";
            }else{
                //std::cout << "End of run procedure\n";
                ResetLocalValues();
                m_context.pop();
                if (m_context.size() > 0){
                  //  std::cout << "Previous procedure!\n";
                   // std::cout << "Goback to  " << m_symbolITS[m_context.top().sequenceRunner] << "\n";
                    m_justSwitchedContext = true;
                    RunInstruction(m_context.top());
                }
                return;
            }
        }else{
            std::cout << "Erro no sequence "<<m_symbolITS[ec.sequenceRunner]<<"\n";
        }
    }

    if (ParseInstruction()){
        RunInstruction(ec);
    }
    m_justSwitchedContext = false;

}

void AnimationScript::ResetLocalValues(){
    m_internalTimer = 0.0f;
}

void AnimationScript::ParseTime(bool &terminate, bool &nextInstruction){
    nextInstruction = true;
    m_BaseAnimationTime = currentInstruction->data.time;
    if (m_BaseAnimationTime <= 0){
        m_BaseAnimationTime = 0.001f;
    }
   // std::cout << "Animation time is now "<<m_BaseAnimationTime<<"\n";
}

void AnimationScript::ParseRange(bool &terminate, bool &nextInstruction){
    terminate = false;
    if (m_justSwitchedCode){
        m_FrameCounter = currentInstruction->GetBegin();
        //std::cout << "Initiated animation on frame: " << m_FrameCounter << " with animation time "<<m_BaseAnimationTime<<"\n";
    }
    m_internalTimer += m_currentDt;
    while (m_internalTimer >= m_BaseAnimationTime){
        m_internalTimer -= m_BaseAnimationTime;
        if (m_FrameCounter >= currentInstruction->GetEnd() || m_FrameCounter >= m_MaxFrames){
            terminate = true;
            nextInstruction = true;
            m_internalTimer = 0;
            m_animationOver = true;
            //std::cout << "End of animation due: "<<(m_FrameCounter >= currentInstruction->GetEnd())<<" : "<<(m_FrameCounter >= m_MaxFrames)<< "\n";
        }else{
            ++m_FrameCounter;
            m_canUpdateFrames = true;
            //std::cout << "New frame: " << m_FrameCounter << " and remaining counter: "<<m_internalTimer<<"\n";
        }
    }
}

void AnimationScript::AddCallback(std::string labelName,std::function<bool()> &cb){
    uint32_t symb = QuerySymbol(labelName);
    m_callbacks[symb] = cb;
}


std::map<uint32_t,std::function<bool()> > m_callbacks;

void AnimationScript::ParseLine(bool &terminate, bool &nextInstruction){
    nextInstruction = true;
    m_LineCounter = currentInstruction->data.labelId;
    //std::cout << "Animation line: "<<m_LineCounter<<"\n";
}

void AnimationScript::ParseJump(bool &terminate, bool &nextInstruction){
    terminate = true;
    nextInstruction = true;

    if (!m_justSwitchedContext){
        uint32_t label = currentInstruction->data.labelId;
        if (m_instructions.find(label) != m_instructions.end()){
            executionContext ec;
            ec.programCounter = 0;
            ec.sequenceRunner = label;
            m_context.push(ec);
            m_yieldToTop = true;
        }else{
            std::cout << "Error on instruction sequence "<<m_symbolITS[label]<<"\n";
        }
        m_innerCounter--;
    }




}

/**
    terminate true means kill this instruction, and go to next
    terminate false, means repeat this instruction


    nextInstruction true, in the same frame, do the next instruction
    nextInstruction false, do the next instruction (or current), in the next frame

*/

void AnimationScript::ParseLoop(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    nextInstruction = true;
    m_context.top().programCounter = 0;
}

void AnimationScript::ParseCallback(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    nextInstruction = true;
    terminate = true;
    uint32_t cbId = currentInstruction->data.labelId;
    auto it = m_callbacks.find(cbId);
    if (it == m_callbacks.end()){
        std::cout << "Unknow callback\n";
    }else{
        m_lastCallbackValue = m_callbacks[cbId]();
    }
}

void AnimationScript::ParseConditionJump(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    nextInstruction = true;
    terminate = true;
    if (m_lastCallbackValue){
        m_context.pop();
        uint32_t label = currentInstruction->data.labelId;

        if (m_instructions.find(label) != m_instructions.end()){
            executionContext ec;
            ec.programCounter = 0;
            ec.sequenceRunner = label;
            m_context.push(ec);
        }else{
            std::cout << "Error no instruction sequence "<<m_symbolITS[label]<<"\n";
        }
    }
}

//OPCODE_CONDITIONJUMP
void AnimationScript::ParseAuxiliar(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    terminate = false;
    nextInstruction = true;

    //std::cout << "INNER IS NOW: "<<m_innerCounter<<"\n";
    if (m_innerCounter > 0){
        executionContext ec;
        ec.programCounter = 0;
        ec.sequenceRunner = currentInstruction->data.labelId;;
       // std::cout << "->>>>>>>>>>>>>>>>>" << m_symbolITS[currentInstruction->data.labelId] << "\n";
        m_context.push(ec);
        m_yieldToTop = true;
    }else{
        //std::cout << "Terminated bro!\n";
        terminate = true;
    }
    m_innerCounter--;

}

void AnimationScript::ParseWait(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    terminate = false;
    if (m_justSwitchedCode){
        m_internalTimer = currentInstruction->data.time;
        //std::cout << "Initiated animation on frame: " << m_FrameCounter << " with animation time "<<m_BaseAnimationTime<<"\n";
    }
    m_internalTimer -= m_currentDt;
    if (m_internalTimer <= 0){
        terminate = true;
        nextInstruction = true;
    }
}

void AnimationScript::ParseRepeat(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    terminate = true;
    nextInstruction = true;
    m_innerCounter = currentInstruction->data.labelId;
    /*

    executionContext ec;
    ec.programCounter = 0;
    ec.sequenceRunner = label;
    m_context.push(ec);*/
}

bool AnimationScript::ParseInstruction(){
    bool terminate = true;
    bool nextInstruction = false;
    //std::cout <<"--Ins--\n";
    switch (static_cast<AnimInstructionOpcode>(currentInstruction->opcode)){
        case OPCODE_NOP:
            //std::cout << "[opcode]NOP\n";
            nextInstruction = true;
            break;
        case OPCODE_LINE:
            //std::cout << "[opcode]LINE\n";
            ParseLine(terminate, nextInstruction);
            break;
        case OPCODE_TIME:
            //std::cout << "[opcode]TIME\n";
            ParseTime(terminate, nextInstruction);
            break;
        case OPCODE_RANGE:
            //std::cout << "[opcode]OPCODE_RANGE\n";
            ParseRange(terminate, nextInstruction);
            break;
        case OPCODE_JUMP:
            //std::cout << "[opcode]OPCODE_JUMP\n";
            ParseJump(terminate, nextInstruction);
            break;
        case OPCODE_LOOP:
            //std::cout << "[opcode]OPCODE_LOOP\n";
            ParseLoop(terminate, nextInstruction);
            break;
        case OPCODE_REPEAT:
            //std::cout << "[opcode]OPCODE_REPEAT\n";
            ParseRepeat(terminate, nextInstruction);
            break;
        case OPCODE_TOEND:
            //std::cout << "[opcode]OPCODE_TOEND\n";
            ParseAuxiliar(terminate, nextInstruction);
            break;
        case OPCODE_WAIT:
            //std::cout << "[opcode]OPCODE_TOEND\n";
            ParseWait(terminate, nextInstruction);
            break;
        case OPCODE_CONDITIONJUMP:
            //std::cout << "[opcode]OPCODE_TOEND\n";
            ParseConditionJump(terminate, nextInstruction);
            break;
        case OPCODE_CALLBACK:
        case OPCODE_CONDITIONCALLBACK:
            //std::cout << "[opcode]OPCODE_TOEND\n";
            ParseCallback(terminate, nextInstruction);
            break;
        default:
            std::cout << "Unknow opcode: "<<(int)currentInstruction->opcode<<"\n";
            nextInstruction = false;
            break;
    }
   // std::cout <<"--done--\n";
    if (terminate){
        currentInstruction = nullptr;
    }
    return nextInstruction;
}

void AnimationScript::Update(float dt){
    //bear::out << "Frame.\n";
    if (m_context.size() > 0){
        m_currentDt = dt;
        RunInstruction(m_context.top());
    }
    //std::cout << "----------end-----------\n";
}

uint32_t AnimationScript::QuerySymbol(std::string& symb){
    if (m_symbolSTI.find(symb) != m_symbolSTI.end()){
        return m_symbolSTI[symb];
    }
    return AddSymbol(std::move(symb));
}

uint32_t AnimationScript::AddSymbol(std::string&& symb){
    ++m_symbolCounter;
    m_symbolSTI[symb] = m_symbolCounter;
    m_symbolITS[m_symbolCounter] = symb;
    std::cout << "Added symbol " << symb << " as " << m_symbolCounter << "\n";
    return m_symbolCounter;
}


void AnimationScript::AddInstructionSet(uint32_t id,InstructionSet &instructions){
    if (m_instructions.find(id) != m_instructions.end()){
        throw BearException(utils::format("Double reference for '%s'",m_symbolITS[id] ) );
        return;
    }
    m_instructions[id] = instructions;
}

bool AnimationScript::LoadFile(std::string path){

/*
    //std::string fdata = "UWU		:	t4.0,	0..4, UWU";
    uint32_t symb = AddSymbol("UWU");

    InstructionSet iset;
    iset.emplace_back(AnimInstruction(OPCODE_TIME, 0.15f));
    iset.emplace_back(AnimInstruction(OPCODE_RANGE, 0, 4));
    iset.emplace_back(AnimInstruction(OPCODE_JUMP, symb));

    m_instructions[symb] = iset;

    //SEQ		:	L2,		t1.0,	0..4, l3, 0...
    symb = AddSymbol("owo");

    InstructionSet iset2;
    iset2.emplace_back(AnimInstruction(OPCODE_LINE, (uint32_t)2));
    iset2.emplace_back(AnimInstruction(OPCODE_TIME, 0.1f));
    iset2.emplace_back(AnimInstruction(OPCODE_RANGE, 0, 4));
    iset2.emplace_back(AnimInstruction(OPCODE_LINE, (uint32_t)3));
    iset2.emplace_back(AnimInstruction(OPCODE_TOEND, (uint32_t)0));

    m_instructions[symb] = iset2;*/

    return true;
}
