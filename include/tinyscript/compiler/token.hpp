//
//  Token.hpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <tinyscript/opcodes.hpp>

namespace tinyscript {
    
    struct Token {
        
        enum class OperatorType {
            Arithmetic,
            String,
            Relational,
            Logical,
            Assignment
        };
        
        enum class Kind {
            brace_l,
            brace_r,
            bracket_l,
            bracket_r,
            paren_l,
            paren_r,
            
            op_dot,
            
            op_amp,
            
            op_eq,
            op_plus,
            op_minus,
            op_star,
            op_slash,
            
            op_lt,
            op_gt,
            op_lteq,
            op_gteq,
            op_eqeq,
            
            comma,
            colon,
            semicolon,
            arrow,
            
            kw_var,
            kw_func,
            kw_if,
            kw_guard,
            kw_else,
            kw_loop,
            kw_until,
            kw_or,
            kw_and,
            kw_next,
            kw_stoploop,
            kw_return,
            kw_yield,
            kw_exit,
            kw_fail,
            kw_yes,
            kw_no,
            
            ty_integer,
            ty_real,
            ty_string,
            ty_bool,
            ty_void,
            
            identifier,
            lit_integer,
            lit_floating,
            lit_string,
            
            eof,
            invalid,
        };
        
        static Kind identifierKind(const char* symbol, std::uint32_t length);
        static std::string kindString(Kind kind);
        
        Kind            kind;
        bool            isStartOfLine;
        std::uint32_t   location;
        std::uint32_t   length;
        
        bool isUnaryOp() const;
        bool isBinaryOp() const;
        
        OperatorType operatorType() const;
        Opcode operatorInstruction() const;
        
        bool binaryRightAssociative() const;
        int binaryPrecedence() const;
    };
}
