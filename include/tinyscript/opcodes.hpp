//
//  opcodes.hpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

#pragma once
#include <string>
#include <iostream>

namespace tinyscript {
#define OPCODE(name, _, __) name,
    enum Opcode {
#include <tinyscript/x-opcodes.hpp>
    };
#undef OPCODE
    
    std::string mnemonic(Opcode code);
    int stackEffect(Opcode code);
    int operandSize(Opcode code);
}

std::ostream& operator<<(std::ostream& oit, tinyscript::Opcode code);
