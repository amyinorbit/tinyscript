//
//  Compiler.cpp
//  mvm
//
//  Created by Amy Parent on 02/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <iostream>
#include <tinyscript/compiler/compiler.hpp>

#include <tinyscript/compiler/sourcemanager.hpp>
#include <tinyscript/runtime/vm.hpp>

namespace tinyscript {
    
    using std::string;
    
    Compiler::Compiler(const VM& vm, const SourceManager& manager)
    : manager_(manager)
    , vm_(vm)
    , scanner_(manager)
    , sema_(vm, manager)
    , codegen_(manager_) {
        
    }
    
    Program Compiler::compile(bool dump) {
        scanner_.consumeToken();
        recProgram();
        codegen_.emitInstruction(Opcode::halt);
        return codegen_.generate(dump);
    }
    
    void Compiler::compilerError(const std::string &message) {
        if(recovering) return;
        std::cerr << "error: " << message << std::endl;
        manager_.printLineAndToken(std::cerr, current().location, current().length);
        std::cerr << std::endl;
        recovering = true;
    }
    
    // MARK: - Recursive Descent utilities
    
    bool Compiler::have(Token::Kind kind) const {
        return scanner_.currentToken().kind == kind;
    }
    
    bool Compiler::haveConditional() const {
        auto kind = scanner_.currentToken().kind;
        return kind == Token::Kind::kw_loop || kind == Token::Kind::kw_until || kind == Token::Kind::kw_if;
    }
    
    bool Compiler::haveFlowStatement() const {
        auto kind = scanner_.currentToken().kind;
        return kind == Token::Kind::kw_exit
            || kind == Token::Kind::kw_yield
            || kind == Token::Kind::kw_next
            || kind == Token::Kind::kw_stoploop
            || kind == Token::Kind::kw_fail;
    }
    
    bool Compiler::haveTerm() const {
        auto kind = scanner_.currentToken().kind;
        return kind == Token::Kind::op_plus
            || kind == Token::Kind::op_minus
            || kind == Token::Kind::paren_l
            || kind == Token::Kind::identifier
            || kind == Token::Kind::bracket_l
            || kind == Token::Kind::kw_no
            || kind == Token::Kind::kw_yes
            || kind == Token::Kind::lit_integer
            || kind == Token::Kind::lit_floating
            || kind == Token::Kind::lit_string;
    }
    
    bool Compiler::haveUnary() const {
        return scanner_.currentToken().isUnaryOp();
    }
    
    bool Compiler::haveBinary() const {
        return scanner_.currentToken().isBinaryOp();
    }
    
    bool Compiler::implicitTerminator() const {
        const auto& tok = scanner_.currentToken();
        return tok.isStartOfLine || tok.kind == Token::Kind::eof || tok.kind == Token::Kind::brace_r;
    }
    
    bool Compiler::match(Token::Kind kind) {
        if(have(kind)) {
            scanner_.consumeToken();
            return true;
        }
        return false;
    }
    
    bool Compiler::expect(Token::Kind kind) {
        if(recovering) {
            while(!have(kind) && !have(Token::Kind::eof)) scanner_.consumeToken();
            if(have(Token::Kind::eof)) return false;
            recovering = false;
            return match(kind);
        }
        if(match(kind)) return true;
        compilerError("Unexpected token");
        recovering = true;
        return false;
    }
    
    bool Compiler::expectTerminator() {
        if(recovering) {
            while(!have(Token::Kind::semicolon) && !implicitTerminator() && !have(Token::Kind::eof))
                scanner_.consumeToken();
            if(have(Token::Kind::eof)) return false;
            recovering = false;
            return have(Token::Kind::semicolon) ? match(Token::Kind::semicolon) : true;
        }
        if(match(Token::Kind::semicolon) || implicitTerminator()) return true;
        compilerError("consecutive statement on a line must be separated by ';'");
        recovering = true;
        return false;
    }
}

