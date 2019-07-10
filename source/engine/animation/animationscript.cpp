#include "animationscript.hpp"

#include "../../framework/utils.hpp"
#include <iostream>


void AnimationLoader::Run(std::string Label){
    if (m_symbolSTI.find(Label) != m_symbolSTI.end()){
        executionContext ec;
        ec.programCounter = 0;
        ec.sequenceRunner = m_symbolSTI[Label];
        std::cout << "Added sequence " << ec.sequenceRunner << " : "<<Label<<"\n";
        if (m_context.size() > 0)
            m_context.pop();
        currentInstruction = nullptr;
        m_context.push(ec);
    }
}

void AnimationLoader::RunInstruction(executionContext &ec){
    m_justSwitchedCode = false;
    m_animationOver = false;
    m_canUpdateFrames = false;
    if (currentInstruction == nullptr){
        std::map<uint32_t,InstructionSet>::iterator it = m_instructions.find(ec.sequenceRunner);
        if (it != m_instructions.end()){
            if (it->second.size() > ec.programCounter){
                currentInstruction = &it->second[ec.programCounter];
                ++ec.programCounter;
                m_justSwitchedCode = true;
                std::cout << "Increased PC to "<< ec.programCounter << "\n";
            }else{
                std::cout << "End of run procedure\n";
                ResetLocalValues();
                m_context.pop();
                return;
            }
        }else{
            std::cout << "Erro no sequence "<<m_symbolITS[ec.sequenceRunner]<<"\n";
        }
    }
    if (ParseInstruction()){
        RunInstruction(ec);
    }

}

void AnimationLoader::ResetLocalValues(){
    m_internalTimer = 0.0f;
}

void AnimationLoader::ParseTime(bool &terminate, bool &nextInstruction){
    nextInstruction = true;
    m_BaseAnimationTime = currentInstruction->data.time;
    if (m_BaseAnimationTime <= 0){
        m_BaseAnimationTime = 0.001f;
    }
    std::cout << "Animation time is now "<<m_BaseAnimationTime<<"\n";
}

void AnimationLoader::ParseRange(bool &terminate, bool &nextInstruction){
    terminate = false;
    if (m_justSwitchedCode){
        m_FrameCounter = currentInstruction->GetBegin();
        std::cout << "Initiated animation on frame: " << m_FrameCounter << " with animation time "<<m_BaseAnimationTime<<"\n";
    }
    m_internalTimer += m_currentDt;
    while (m_internalTimer >= m_BaseAnimationTime){
        m_internalTimer -= m_BaseAnimationTime;
        if (m_FrameCounter >= currentInstruction->GetEnd() || m_FrameCounter >= m_MaxFrames){
            terminate = true;
            nextInstruction = true;
            m_internalTimer = 0;
            m_animationOver = true;
            std::cout << "End of animation\n";
        }else{
            ++m_FrameCounter;
            m_canUpdateFrames = true;
            std::cout << "New frame: " << m_FrameCounter << " and remaining counter: "<<m_internalTimer<<"\n";
        }
    }
}

void AnimationLoader::ParseLine(bool &terminate, bool &nextInstruction){
    nextInstruction = true;
    m_LineCounter = currentInstruction->data.labelId;
    std::cout << "Animation line: "<<m_LineCounter<<"\n";
}

void AnimationLoader::ParseJump(bool &terminate, bool &nextInstruction){
    nextInstruction = true;
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

bool AnimationLoader::ParseInstruction(){
    bool terminate = true;
    bool nextInstruction = false;
    switch (static_cast<AnimInstructionOpcode>(currentInstruction->opcode)){
        case OPCODE_NOP:
            std::cout << "[opcode]NOP\n";
            nextInstruction = true;
            break;
        case OPCODE_LINE:
            std::cout << "[opcode]LINE\n";
            ParseLine(terminate, nextInstruction);
            break;
        case OPCODE_TIME:
            std::cout << "[opcode]TIME\n";
            ParseTime(terminate, nextInstruction);
            break;
        case OPCODE_RANGE:
            std::cout << "[opcode]OPCODE_RANGE\n";
            ParseRange(terminate, nextInstruction);
            break;
        case OPCODE_TOEND:
            std::cout << "[opcode]OPCODE_TOEND\n";
            ParseRange(terminate, nextInstruction);
            break;
        case OPCODE_JUMP:
            std::cout << "[opcode]OPCODE_JUMP\n";
            ParseJump(terminate, nextInstruction);
            break;
        default:
            std::cout << "Unknow opcode\n";
            nextInstruction = false;
            break;
    }
    if (terminate){
        currentInstruction = nullptr;
    }
    return nextInstruction;
}

void AnimationLoader::Update(float dt){
    if (m_context.size() > 0){
        m_currentDt = dt;
        RunInstruction(m_context.top());
    }
}

uint32_t AnimationLoader::QuerySymbol(std::string& symb){
    if (m_symbolSTI.find(symb) != m_symbolSTI.end()){
        return m_symbolSTI[symb];
    }
    return AddSymbol(std::move(symb));
}

uint32_t AnimationLoader::AddSymbol(std::string&& symb){
    ++m_symbolCounter;
    m_symbolSTI[symb] = m_symbolCounter;
    m_symbolITS[m_symbolCounter] = symb;
    std::cout << "Added symbol " << symb << " as " << m_symbolCounter << "\n";
    return m_symbolCounter;
}


void AnimationLoader::AddInstructionSet(uint32_t id,InstructionSet &instructions){
    if (m_instructions.find(id) != m_instructions.end()){
        throw BearException(utils::format("Double reference for '%s'",m_symbolITS[id] ) );
        return;
    }
    m_instructions[id] = instructions;
}

bool AnimationLoader::LoadFile(std::string path){


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

    m_instructions[symb] = iset2;

    return true;
}
