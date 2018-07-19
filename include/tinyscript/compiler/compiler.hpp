//
//  Compiler.hpp
//  mvm
//
//  Created by Amy Parent on 02/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <string>
#include <utility>

#include <tinyscript/compiler/token.hpp>
#include <tinyscript/compiler/scanner.hpp>
#include <tinyscript/compiler/sema.hpp>
#include <tinyscript/compiler/typeexpr.hpp>
#include <tinyscript/compiler/codegen.hpp>
#include <tinyscript/runtime/program.hpp>

namespace tinyscript {
    class VM;
    class SourceManager;
    class Scanner;
    
    class Compiler {
    public:
        Compiler(const VM& vm, const SourceManager& manager);
        Program compile(bool dump = false);
        
    private:
        
        // MARK: - recursive descent recognizers;
        
        void recProgram();
        void recBlock();
        
        void recCountLoop();
        void recUntilLoop();
        void recGuard();
        void recIfElse();
        void recVarDecl();
        void recFuncDecl();
        std::pair<Token, Type> recParamDecl();
        Type recTypeDecl();
        void recAssign();
        void recFlowStatement();
        void recYield();
        
        TypeExpr recExpression(int level);
        TypeExpr recTerm();
        TypeExpr recFuncCall();
        
        // MARK: - recursive descent utilities
        
        void compilerError(const std::string& message);
        
        const Token& current() const { return scanner_.currentToken(); }
        
        bool have(Token::Kind kind) const;
        bool haveFlowStatement() const;
        bool haveConditional() const;
        bool haveTerm() const;
        bool haveUnary() const;
        bool haveBinary() const;
        bool implicitTerminator() const;
        
        bool match(Token::Kind kind);
        
        bool expect(Token::Kind kind);
        bool expectTerminator();
        
        bool recovering = false;
        
        
        const SourceManager& manager_;
        const VM& vm_;
        Scanner scanner_;
        Sema sema_;
        CodeGen codegen_;
    };
}
