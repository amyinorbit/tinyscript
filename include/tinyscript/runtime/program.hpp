//
//  program.hpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>

#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    class Program {
    public:
        struct Function {
            std::uint8_t                variableCount;
            std::vector<std::uint8_t>   bytecode;
        };
        
        using FunctionTable = std::unordered_map<std::string, Function>;
        
        Function                    script;
        FunctionTable               functions;
        std::vector<Value>          constants;
        std::vector<std::uint8_t>   bytecode;
        std::uint16_t               variableCount;
    };
}
