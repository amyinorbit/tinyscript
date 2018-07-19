//
//  token.cpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <string>
#include <map>
#include <tinyscript/compiler/token.hpp>

namespace tinyscript {
    
    struct TokenEntry {
        Token::Kind kind;
        std::string name;
    };
    
    static const TokenEntry keywords[] = {
        {Token::Kind::kw_var,       "var"},
        {Token::Kind::kw_func,      "func"},
        {Token::Kind::kw_if,        "if"},
        {Token::Kind::kw_guard,     "guard"},
        {Token::Kind::kw_else,      "else"},
        {Token::Kind::kw_until,     "until"},
        {Token::Kind::kw_loop,      "loop"},
        {Token::Kind::kw_or,        "or"},
        {Token::Kind::kw_and,       "and"},
        {Token::Kind::kw_next,      "next"},
        {Token::Kind::kw_stoploop,  "stoploop"},
        {Token::Kind::kw_return,    "return"},
        {Token::Kind::kw_yield,     "yield"},
        {Token::Kind::kw_exit,      "exit"},
        {Token::Kind::kw_fail,      "fail"},
        {Token::Kind::kw_yes,       "yes"},
        {Token::Kind::kw_no,        "no"},
        
        {Token::Kind::ty_integer,   "Integer"},
        {Token::Kind::ty_real,      "Real"},
        {Token::Kind::ty_string,    "String"},
        {Token::Kind::ty_bool,      "Bool"},
        {Token::Kind::ty_void,      "Void"},
    };
    
    struct OperatorData {
        int precedence;
        bool rightAssociative;
        Token::OperatorType type;
        Opcode instr;
    };
    
    static const std::map<Token::Kind, OperatorData> operators = {
        {Token::Kind::op_star,      {90, false, Token::OperatorType::Arithmetic,    Opcode::fmul}},
        {Token::Kind::op_slash,     {00, false, Token::OperatorType::Arithmetic,    Opcode::fdiv}},
        {Token::Kind::op_amp,       {80, false, Token::OperatorType::String,        Opcode::sadd}},
        {Token::Kind::op_plus,      {80, false, Token::OperatorType::Arithmetic,    Opcode::fadd}},
        {Token::Kind::op_minus,     {80, false, Token::OperatorType::Arithmetic,    Opcode::fsub}},
        {Token::Kind::op_gt,        {60, false, Token::OperatorType::Relational,    Opcode::test_fgt}},
        {Token::Kind::op_lt,        {60, false, Token::OperatorType::Relational,    Opcode::test_flt}},
        {Token::Kind::op_gteq,      {60, false, Token::OperatorType::Relational,    Opcode::test_fgt}}, // TODO: Fix
        {Token::Kind::op_lteq,      {60, false, Token::OperatorType::Relational,    Opcode::test_flt}}, // TODO: Fix
        {Token::Kind::op_eqeq,      {50, false, Token::OperatorType::Relational,    Opcode::test_feq}},
        {Token::Kind::kw_and,       {10, false, Token::OperatorType::Logical,       Opcode::log_and}},
        {Token::Kind::kw_or,        {9,  false, Token::OperatorType::Logical,       Opcode::log_or}},
        {Token::Kind::op_eq,        {0,  false, Token::OperatorType::Assignment,    Opcode::store}},
    };
    
    bool Token::isUnaryOp() const {
        return kind == Kind::op_minus;
    }
    
    bool Token::isBinaryOp() const {
        return operators.find(kind) != operators.end();
    }
    
    Token::OperatorType Token::operatorType() const {
        assert(isBinaryOp() && "Not an operator");
        return operators.at(kind).type;
    }
    
    Opcode Token::operatorInstruction() const {
        assert(isBinaryOp() && "Not an operator");
        return operators.at(kind).instr;
    }
    
    bool Token::binaryRightAssociative() const {
        assert(isBinaryOp() && "Not an operator");
        return operators.at(kind).rightAssociative;
    }
    
    int Token::binaryPrecedence() const {
        assert(isBinaryOp() && "Not an operator");
        return operators.at(kind).precedence;
    }
    
    Token::Kind Token::identifierKind(const char *symbol, std::uint32_t length) {
        std::string needle(symbol, length);
        for(const auto& entry: keywords) {
            if(needle == entry.name) { return entry.kind; }
        }
        return Token::Kind::identifier;
    }
}
