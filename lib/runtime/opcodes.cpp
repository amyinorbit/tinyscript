//
//  opcodes.cpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <string>
#include <tinyscript/opcodes.hpp>

namespace tinyscript {
    struct OpcodeData {
        std::string mnemonic;
        int stackEffect;
        int operandSize;
    };
#define OPCODE(name, effect, opsize) {#name, effect, opsize},
    static const OpcodeData opcodeData[] = {
#include <tinyscript/x-opcodes.hpp>
    };
#undef OPCODE
    
    std::string mnemonic(Opcode code) {
        return opcodeData[code].mnemonic;
    }
    
    int stackEffect(Opcode code) {
        return opcodeData[code].stackEffect;
    }
    
    int operandSize(Opcode code) {
        return opcodeData[code].operandSize;
    }
}

std::ostream& operator<<(std::ostream& out, tinyscript::Opcode code) {
    out << mnemonic(code);
    return out;
}
