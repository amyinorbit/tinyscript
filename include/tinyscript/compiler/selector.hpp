//
//  selector.hpp
//  tinyscript
//
//  Created by Amy Parent on 14/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <string>

#include <tinyscript/type.hpp>
#include <tinyscript/opcodes.hpp>
#include <tinyscript/compiler/codegen.hpp>

namespace tinyscript {
    struct Token;
    class VM;
    
    namespace Selector {
        
        struct Conversion {
        public:
            enum class Kind {None, IntFloat, FloatInt, Call};
            
            static Conversion None();
            static Conversion I2F();
            static Conversion F2I();
            static Conversion String();
            
            bool required() const { return kind_ == Kind::None; }
            void emit(CodeGen& codegen, ILBuilder::InsertLocation at);
        private:
            Conversion(Kind kind, const std::string& module = "", const std::string& func = "") : kind_(kind), module_(module), func_(func) {}
            Kind kind_;
            std::string module_;
            std::string func_;
        };
        
        Opcode unaryInstruction(const Token& op, Type operands);
        Opcode binaryInstruction(const Token& op, Type operand);
        
        Conversion convert(Type source, Type target);
    }
}

