//
//  recognizers.cpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <tinyscript/compiler/compiler.hpp>
#include <tinyscript/compiler/sourcemanager.hpp>
#include <tinyscript/compiler/selector.hpp>
#include <tinyscript/runtime/vm.hpp>

namespace tinyscript {
    void Compiler::recProgram() {
        recBlock();
        expect(Token::Kind::eof);
    }
    
    void Compiler::recBlock() {
        sema_.pushScope();
        for(;;) {
            if(have(Token::Kind::kw_var))
                recVarDecl();
            else if(haveTerm())
                recExpression(0);
            else if(have(Token::Kind::kw_until))
                recUntilLoop();
            else if(have(Token::Kind::kw_loop))
                recCountLoop();
            else if(have(Token::Kind::kw_if))
                recIfElse();
            else if(have(Token::Kind::kw_guard))
                recGuard();
            else if(haveFlowStatement())
                recFlowStatement();
            else
                break;
            expectTerminator();
        }
        sema_.popScope();
    }
    
    void Compiler::recCountLoop() {
        expect(Token::Kind::kw_loop);
        Token loopCount = scanner_.currentToken();
        expect(Token::Kind::lit_integer);
        
        codegen_.openLoop();
        codegen_.emitConstantI(Opcode::load_c, loopCount);
        codegen_.emitLocal(Opcode::store, codegen_.loopVariable());
        
        codegen_.emitLabel(codegen_.loopLabel());
        codegen_.emitLocal(Opcode::load, codegen_.loopVariable());
        codegen_.emitConstantI(Opcode::load_c, 0);
        codegen_.emitInstruction(Opcode::test_ieq);
        codegen_.emitJump(Opcode::jnz, codegen_.endLoopLabel());
        
        expect(Token::Kind::brace_l);
        recBlock();
        expect(Token::Kind::brace_r);
        
        codegen_.emitLocal(Opcode::load, codegen_.loopVariable());
        codegen_.emitConstantI(Opcode::load_c, 1);
        codegen_.emitInstruction(Opcode::isub);
        codegen_.emitLocal(Opcode::store, codegen_.loopVariable());
        codegen_.emitJump(Opcode::rjmp, codegen_.loopLabel());
        
        codegen_.emitLabel(codegen_.endLoopLabel());
    }
    
    void Compiler::recUntilLoop() {
        Token statement = current();
        expect(Token::Kind::kw_until);
        codegen_.openLoop();
        codegen_.emitLabel(codegen_.loopLabel());
        
        if(!recExpression(0).is(Type::Bool)) {
            sema_.semanticError(statement, "conditional statements work on boolean expressions");
        }
        codegen_.emitJump(Opcode::jnz, codegen_.endLoopLabel());
        
        expect(Token::Kind::brace_l);
        recBlock();
        expect(Token::Kind::brace_r);
        
        codegen_.emitJump(Opcode::rjmp, codegen_.loopLabel());
        codegen_.emitLabel(codegen_.endLoopLabel());
        codegen_.closeLoop();
    }
    
    void Compiler::recGuard() {
        Token statement = current();
        expect(Token::Kind::kw_guard);
        if(!recExpression(0).is(Type::Bool)) {
            sema_.semanticError(statement, "guard statements work on boolean expressions");
        }
        
        codegen_.openIf();
        codegen_.emitJump(Opcode::jnz, codegen_.endifLabel());
        
        expect(Token::Kind::kw_else);
        recFlowStatement();
        
        codegen_.emitLabel(codegen_.endifLabel());
        codegen_.closeIf();
    }
    
    void Compiler::recIfElse() {
        Token statement = current();
        expect(Token::Kind::kw_if);
        if(!recExpression(0).is(Type::Bool)) {
            sema_.semanticError(statement, "conditional statements work on boolean expressions");
        }
        
        codegen_.openIf();
        codegen_.emitJump(Opcode::jnz, codegen_.ifLabel());
        codegen_.emitJump(Opcode::jmp, codegen_.elseLabel());
        codegen_.emitLabel(codegen_.ifLabel());
        
        expect(Token::Kind::brace_l);
        recBlock();
        expect(Token::Kind::brace_r);
        
        codegen_.emitJump(Opcode::jmp, codegen_.endifLabel());
        codegen_.emitLabel(codegen_.elseLabel());
        
        if(!have(Token::Kind::kw_else)) {
            codegen_.emitLabel(codegen_.endifLabel());
            return;
        }
        expect(Token::Kind::kw_else);
        if(have(Token::Kind::brace_l)) {
            expect(Token::Kind::brace_l);
            recBlock();
            expect(Token::Kind::brace_r);
        }
        else if(have(Token::Kind::kw_if)) {
            recIfElse();
        } else {
            compilerError("expected a code block or 'if'");
        }
        codegen_.emitLabel(codegen_.endifLabel());
    }
    
    void Compiler::recVarDecl() {
        expect(Token::Kind::kw_var);
        Token name = current();
        expect(Token::Kind::identifier);
        expect(Token::Kind::op_eq);
        auto type = recExpression(0);
        
        sema_.declareVariable(name, type.unqualifiedType());
        codegen_.declareLocal(name);
        codegen_.emitLocal(Opcode::store, name);
    }
    
    // void Compiler::recAssign() {
    //     Token symbol = current();
    //     expect(Token::Kind::identifier);
    //     Token eq = current();
    //     expect(Token::Kind::op_eq);
    //     sema_.binaryOpType(eq, sema_.getVarType(symbol), recExpression(0));
    //     codegen_.emitLocal(Opcode::store, symbol);
    // }
    
    void Compiler::recFlowStatement() {
        if(have(Token::Kind::kw_next)) {
            expect(Token::Kind::kw_next);
            codegen_.emitJump(Opcode::rjmp, codegen_.loopLabel());
        }
        else if(have(Token::Kind::kw_stoploop)) {
            expect(Token::Kind::kw_stoploop);
            codegen_.emitJump(Opcode::jmp, codegen_.endLoopLabel());
        }
        else if(have(Token::Kind::kw_yield)) {
            recYield();
        }
        else if(have(Token::Kind::kw_exit)) {
            expect(Token::Kind::kw_exit);
            codegen_.emitInstruction(Opcode::halt);
        }
        else if(have(Token::Kind::kw_fail)) {
            expect(Token::Kind::kw_fail);
            auto symbol = current();
            expect(Token::Kind::lit_string);
            codegen_.emitConstantS(Opcode::fail, symbol);
        }
        else {
            compilerError("expected a flow control statement");
        }
    }
    
    void Compiler::recYield() {
        match(Token::Kind::kw_yield);
        if(haveTerm()) {
            recExpression(0);
            codegen_.emitInstruction(Opcode::yield_v);
        } else {
            codegen_.emitInstruction(Opcode::yield);
        }
    }
    
    TypeExpr Compiler::recExpression(int min) {
        auto patchAssignRem = codegen_.patchPoint();
        TypeExpr type = recTerm();
        for(;;) {
            auto patchLHS = codegen_.patchPoint();
            if(!haveBinary()) break;
            if(current().binaryPrecedence() < min) break;
            
            Token op = current();
            int prec = op.binaryPrecedence();
            bool right = op.binaryRightAssociative();
            
            int nextMin = right ? prec : prec + 1;
            scanner_.consumeToken();
            
            auto lhs = type;
            auto rhs = recExpression(nextMin);
            auto mapping = sema_.binaryOpType(op, lhs, rhs);
            
            if(op.kind == Token::Kind::op_eq) {
                if(!lhs.isLValue()) {
                    sema_.semanticError(op, "Cannot assign to a non-variable");
                }
                else {
                    codegen_.dropCode(patchAssignRem);
                    Selector::convert(rhs.unqualifiedType(), mapping.from).emit(codegen_, codegen_.patchPoint());
                    codegen_.emitLocal(Opcode::store, type.lvalue());
                }
            } else {
                Selector::convert(lhs.unqualifiedType(), mapping.from).emit(codegen_, patchLHS);
                Selector::convert(rhs.unqualifiedType(), mapping.from).emit(codegen_, codegen_.patchPoint());
                codegen_.emitInstruction(Selector::binaryInstruction(op, mapping.from));
            }
            type = mapping.to;
        }
        return type;
    }
    
    TypeExpr Compiler::recTerm() {
        bool hasUnary = false;
        Token unary;
        if(haveUnary()) {
            unary = current();
            hasUnary = true;
            scanner_.consumeToken();
        }
        
        TypeExpr type = Type::Invalid;
        auto symbol = current();
        
        if(match(Token::Kind::paren_l)) {
            type = recExpression(0);
            expect(Token::Kind::paren_r);
        }
        else if(match(Token::Kind::identifier)) {
            
            if(match(Token::Kind::op_dot)) {
                type = recFuncCall(symbol);
            } else {
                type = sema_.getVarType(symbol);
                codegen_.emitLocal(Opcode::load, symbol);
            }
        }
        else if(match(Token::Kind::lit_floating)) {
            type = Type::Number;
            codegen_.emitConstantF(Opcode::load_c, symbol);
        }
        else if(match(Token::Kind::lit_integer)) {
            type = Type::Integer;
            codegen_.emitConstantI(Opcode::load_c, symbol);
        }
        else if(match(Token::Kind::lit_string)) {
            type = Type::String;
            codegen_.emitConstantS(Opcode::load_c, symbol);
        }
        else {
            compilerError("expected an expression");
            return TypeExpr(Type::Invalid);
        }
        
        if(hasUnary) {
            codegen_.emitInstruction(Selector::unaryInstruction(unary, type.unqualifiedType()));
        }
        
        return type;
    }
    
    TypeExpr Compiler::recFuncCall(const Token& module) {
        
        std::uint8_t arity = 0;
        Token func = current();
        expect(Token::Kind::identifier);
        expect(Token::Kind::paren_l);
        if(haveTerm()) {
            arity = 1;
            recExpression(0);
            while(match(Token::Kind::comma)) {
                arity += 1;
                recExpression(0);
            }
        }
        expect(Token::Kind::paren_r);
        auto type = sema_.getFuncType(module, func, arity);
        codegen_.emitConstantS(Opcode::call_f, VM::mangleFunc(manager_.tokenAsString(module), manager_.tokenAsString(func), arity));
        return type;
    }
}
