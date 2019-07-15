#include "animationscript.hpp"
#include "animation.hpp"

#include "../../framework/utils.hpp"
#include "../sprite.hpp"
#include <iostream>


void AnimationScript::FormatSprite(AnimatedSprite &sp){
    sp.SetGridSize(m_spriteGrid.x, m_spriteGrid.y);
    sp.SetAnimation(m_LineCounter,m_MaxFrames,m_BaseAnimationTime);
}

void AnimationScript::UpdateSprite(Sprite &sp){
    sp.SetClip(m_spriteGrid.x * m_FrameCounter, m_spriteGrid.y * m_LineCounter, m_spriteGrid.x, m_spriteGrid.y);
}

void AnimationScript::UpdateSprite(AnimatedSprite &sp){
    sp.SetClip(m_spriteGrid.x * m_FrameCounter, m_spriteGrid.y * m_LineCounter, m_spriteGrid.x, m_spriteGrid.y);
    if (m_persistenSettings){
        FormatSprite(sp);
    }
}

bool AnimationScript::IsRunning(){
    return m_context.size() > 0;
}

bool AnimationScript::Run(std::string Label){
    if (m_symbolSTI.find(Label) != m_symbolSTI.end()){
        executionContext ec;
        ec.programCounter = 0;
        ec.sequenceRunner = m_symbolSTI[Label];
        if (m_context.size() > 0)
            m_context.pop();
        currentInstruction = nullptr;
        m_context.push(ec);
        return true;
    }
    return false;
}

void AnimationScript::RunInstruction(executionContext &ec){

    if (m_instructionsOnFrame >= 32){
        return;
    }
    m_instructionsOnFrame++;

    if (m_context.size() == 0){
        return;
    }


    m_justSwitchedCode = false;
    m_animationOver = false;
    m_canUpdateFrames = false;


    if (m_yieldToTop){
        m_yieldToTop = false;
        ResetLocalValues();
        ec.programCounter--;
        currentInstruction = nullptr;
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
            }else{
                ResetLocalValues();
                m_context.pop();
                if (m_context.size() > 0){
                    m_justSwitchedContext = true;
                    RunInstruction(m_context.top());
                }
                return;
            }
        }else{
            bear::out << "[Animation script] Erro no sequence "<<m_symbolITS[ec.sequenceRunner]<<"\n";
            Kill();
        }
    }

    if (ParseInstruction()){
        RunInstruction(ec);
    }

}

void AnimationScript::Kill(){
    while (m_context.size() > 0){
        m_context.pop();
    }
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
    }

    m_internalTimer += m_currentDt;
    if (m_internalTimer >= m_BaseAnimationTime){
        m_internalTimer = 0;
        if (m_FrameCounter >= currentInstruction->GetEnd() || m_FrameCounter >= (m_MaxFrames-1) ){
            terminate = true;
            nextInstruction = true;
            m_internalTimer = 0;
            m_animationOver = true;
        }else{
            ++m_FrameCounter;
            m_canUpdateFrames = true;
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
            bear::out << "[Animation script] Error no instruction sequence found named as ["<<m_symbolITS[label]<<"]\n";
            Kill();
        }
        m_innerCounter--;
    }else{
        m_justSwitchedContext = false;
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
        bear::out << "[Animation script] Unknow callback "<<m_symbolITS[cbId]<<"\n";
        Kill();

    }else{
        m_lastCallbackValue = m_callbacks[cbId]();
    }
}

void AnimationScript::ParseConditionJump(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    nextInstruction = true;
    terminate = true;
    if (!m_justSwitchedContext){
        if (m_lastCallbackValue){
            uint32_t label = currentInstruction->data.labelId;

            if (m_instructions.find(label) != m_instructions.end()){
                executionContext ec;
                ec.programCounter = 0;
                ec.sequenceRunner = label;
                m_context.push(ec);
                m_yieldToTop = true;
            }else{
                bear::out << "[Animation script] Error no instruction sequence "<<m_symbolITS[label]<<"\n";
                Kill();
            }
            //m_innerCounter--;
        }
    }else{
        m_justSwitchedContext = false;
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
        m_internalTimer = 0;
    }
    m_internalTimer += m_currentDt;
    if (m_internalTimer >= currentInstruction->data.time){
        terminate = true;
        nextInstruction = true;
    }
}

void AnimationScript::ParseSingleFrame(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    terminate = false;
    nextInstruction = false;
    if (m_justSwitchedCode){
        m_internalTimer = 0;
        m_FrameCounter = currentInstruction->data.labelId;
    }
    m_internalTimer += m_currentDt;
    if (m_internalTimer >= m_BaseAnimationTime){
        terminate = true;
        nextInstruction = true;
    }
}

void AnimationScript::ParseRepeat(bool &terminate /* true*/, bool &nextInstruction /*false*/){
    terminate = true;
    nextInstruction = true;
    m_innerCounter = currentInstruction->data.labelId;
}

bool AnimationScript::ParseInstruction(){
    bool terminate = true;
    bool nextInstruction = false;
    switch (static_cast<AnimInstructionOpcode>(currentInstruction->opcode)){
        case OPCODE_NOP:
            nextInstruction = true;
            break;
        case OPCODE_LINE:
            ParseLine(terminate, nextInstruction);
            break;
        case OPCODE_TIME:
            ParseTime(terminate, nextInstruction);
            break;
        case OPCODE_RANGE:
            ParseRange(terminate, nextInstruction);
            break;
        case OPCODE_SINGLEFRAME:
            ParseSingleFrame(terminate, nextInstruction);
            break;
        case OPCODE_JUMP:
            ParseJump(terminate, nextInstruction);
            break;
        case OPCODE_LOOP:
            ParseLoop(terminate, nextInstruction);
            break;
        case OPCODE_REPEAT:
            ParseRepeat(terminate, nextInstruction);
            break;
        case OPCODE_TOEND:
            ParseAuxiliar(terminate, nextInstruction);
            break;
        case OPCODE_WAIT:
            ParseWait(terminate, nextInstruction);
            break;
        case OPCODE_CONDITIONJUMP:
            ParseConditionJump(terminate, nextInstruction);
            break;
        case OPCODE_CALLBACK:
        case OPCODE_CONDITIONCALLBACK:
            ParseCallback(terminate, nextInstruction);
            break;
        default:
            bear::out << "[Animation script]Unknow opcode: "<<(int)currentInstruction->opcode<<"\n";
            Kill();
            nextInstruction = false;
            break;
    }
    if (terminate){
        currentInstruction = nullptr;
    }
    return nextInstruction;
}

void AnimationScript::Update(float dt){
    m_instructionsOnFrame = 0;
    if (m_context.size() > 0){
        m_currentDt = dt;
        RunInstruction(m_context.top());
    }
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
    return m_symbolCounter;
}


void AnimationScript::AddInstructionSet(uint32_t id,InstructionSet &instructions){
    if (m_instructions.find(id) != m_instructions.end()){
        throw BearException(utils::format("Double referencea for '%s'",m_symbolITS[id] ) );
        return;
    }
    m_instructions[id] = instructions;
}

