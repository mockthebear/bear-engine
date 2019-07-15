#pragma once
#include <vector>
#include <inttypes.h>
#include "../basetypes.hpp"


enum AnimInstructionOpcode{
    OPCODE_NOP = 0,
    OPCODE_RANGE,
    OPCODE_TOEND,
    OPCODE_TIME,
    OPCODE_WAIT,
    OPCODE_LINE,
    OPCODE_CALLBACK,
    OPCODE_REPEAT,
    OPCODE_JUMP,
    OPCODE_CONDITIONCALLBACK,
    OPCODE_CONDITIONJUMP,
    OPCODE_CALL,
    OPCODE_LOOP,
    OPCODE_SINGLEFRAME,
};

typedef struct _AnimInstruction{
    uint8_t opcode;
    union{
        uint16_t frameRange;
        uint32_t labelId;
        float   time;
    } data;

    _AnimInstruction(){
        opcode = 0;
        data.labelId = 0;
    };

    _AnimInstruction(AnimInstructionOpcode op):opcode(static_cast<uint8_t>(op)){
    }

    _AnimInstruction(AnimInstructionOpcode op, float time):opcode(static_cast<uint8_t>(op)){
        data.time = time;
    };

    _AnimInstruction(AnimInstructionOpcode op, uint32_t labelId):opcode(static_cast<uint8_t>(op)){
        data.labelId = labelId;
    };

    _AnimInstruction(AnimInstructionOpcode op, uint16_t frameRange):opcode(static_cast<uint8_t>(op) ){
        data.labelId = frameRange;
    };
    _AnimInstruction(AnimInstructionOpcode op, uint8_t range1, uint8_t range2):opcode(static_cast<uint8_t>(op) ){
        SetBegin(range1);
        SetEnd(range2);
    };

    uint8_t GetBegin(){
        return data.frameRange & 0xff;
    }

    uint8_t GetEnd(){
        return (data.frameRange & 0xff00) >> 8;
    }

    void SetBegin(uint8_t v){
        data.frameRange = (data.frameRange & 0xff00) | (v);
    }

    void SetEnd(uint8_t v){
        data.frameRange = (data.frameRange & 0x00ff) | (v << 8);
    }
} AnimInstruction;

typedef std::vector<AnimInstruction> InstructionSet;
