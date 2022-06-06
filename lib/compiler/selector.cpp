//
//  selector.cpp
//  tinyscript
//
//  Created by Amy Parent on 14/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <unordered_map>
#include <cassert>
#include <tinyscript/compiler/selector.hpp>
#include <tinyscript/compiler/token.hpp>
#include <tinyscript/runtime/vm.hpp>

namespace tinyscript {
    namespace Selector {
        
        Conversion Conversion::None() {
            return Conversion{Kind::None};
        }
        
        Conversion Conversion::I2F() {
            return Conversion{Kind::IntFloat};
        }
        
        Conversion Conversion::F2I() {
            return Conversion{Kind::FloatInt};
        }
        
        Conversion Conversion::String() {
            return Conversion{Kind::Call, "IO", "toString"};
        }
        
        void Conversion::emit(CodeGen &codegen, std::uint64_t at) {
            switch (kind_) {
                case Kind::IntFloat: codegen.patchConversion(Opcode::i2f, at); break;
                case Kind::FloatInt: codegen.patchConversion(Opcode::f2i, at); break;
                case Kind::Call: codegen.patchCall(Opcode::call_f, VM::mangleFunc(module_, func_, 1), at); break;
                default: break;
            }
        }
        
        static const std::unordered_map<Token::Kind,std::unordered_map<Type, Opcode>> unaryOperatorData = {
            {Token::Kind::op_minus, {{Type::Integer, Opcode::imin}, {Type::Number, Opcode::fmin}}},
        };
        
        Opcode unaryInstruction(const Token& op, Type operand) {
            assert(op.isUnaryOp() && "not a binary operator");
            auto it1 = unaryOperatorData.find(op.kind);
            assert(it1 != unaryOperatorData.end() && "Cannot find operator data");
            auto it2 = it1->second.find(operand);
            assert(it2 != it1->second.end() && "Cannot find operator data");
            return it2->second;
        }
        
        static const std::unordered_map<Token::Kind,std::unordered_map<Type, Opcode>> binaryOperatorData = {
            {Token::Kind::op_star,  {{Type::Integer, Opcode::imul}, {Type::Number, Opcode::fmul}}},
            {Token::Kind::op_slash, {{Type::Integer, Opcode::idiv}, {Type::Number, Opcode::fdiv}}},
            {Token::Kind::op_plus,  {{Type::Integer, Opcode::iadd}, {Type::Number, Opcode::fadd}, {Type::String, Opcode::sadd}}},
            {Token::Kind::op_minus, {{Type::Integer, Opcode::isub}, {Type::Number, Opcode::fsub}}},
            {Token::Kind::op_amp,   {{Type::String, Opcode::sadd}}},
            {Token::Kind::op_gt,    {{Type::Integer, Opcode::test_igt}, {Type::Number, Opcode::test_fgt}}},
            {Token::Kind::op_lt,    {{Type::Integer, Opcode::test_ilt}, {Type::Number, Opcode::test_flt}}},
            {Token::Kind::op_gteq,  {{Type::Integer, Opcode::test_igteq}, {Type::Number, Opcode::test_fgteq}}},
            {Token::Kind::op_lteq,  {{Type::Integer, Opcode::test_ilteq}, {Type::Number, Opcode::test_flteq}}},
            {Token::Kind::op_eqeq,  {{Type::Integer, Opcode::test_ieq}, {Type::Number, Opcode::test_feq}, {Type::String, Opcode::test_seq}}},
            {Token::Kind::kw_and,   {{Type::Bool, Opcode::log_and}}},
            {Token::Kind::kw_or,    {{Type::Bool, Opcode::log_or}}},
        };
        
        Opcode binaryInstruction(const Token& op, Type operand) {
            assert(op.isBinaryOp() && "not a binary operator");
            auto it1 = binaryOperatorData.find(op.kind);
            assert(it1 != binaryOperatorData.end() && "Cannot find operator data");
            auto it2 = it1->second.find(operand);
            assert(it2 != it1->second.end() && "Cannot find operator data");
            return it2->second;
        }
        
        Conversion convert(Type source, Type target) {
            if(source == target) return Conversion::None();
            if(source == Type::Integer && target == Type::Number) return Conversion::I2F();
            if(source == Type::Number && target == Type::Integer) return Conversion::F2I();
            if(source != Type::String && target == Type::String) return Conversion::String();
            return Conversion::None();
        }
    }
}
