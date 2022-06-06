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
        for(;;) {
            if(have(Token::Kind::kw_var))
                recVarDecl();
            else if(have(Token::Kind::kw_func))
                recFuncDecl();
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
        sema_.pushScope();
        recBlock();
        sema_.popScope();
        expect(Token::Kind::brace_r);
        
        codegen_.emitLocal(Opcode::load, codegen_.loopVariable());
        codegen_.emitConstantI(Opcode::load_c, 1);
        codegen_.emitInstruction(Opcode::isub);
        codegen_.emitLocal(Opcode::store, codegen_.loopVariable());
        
        codegen_.emitJump(Opcode::rjmp, codegen_.loopLabel());
        codegen_.closeLoop();
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
        sema_.pushScope();
        recBlock();
        sema_.popScope();
        expect(Token::Kind::brace_r);
        
        codegen_.emitJump(Opcode::rjmp, codegen_.loopLabel());
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
        sema_.pushScope();
        recBlock();
        sema_.popScope();
        expect(Token::Kind::brace_r);
        
        codegen_.emitJump(Opcode::jmp, codegen_.endifLabel());
        codegen_.emitLabel(codegen_.elseLabel());
        
        if(!have(Token::Kind::kw_else)) {
            codegen_.closeIf();
            return;
        }
        expect(Token::Kind::kw_else);
        if(have(Token::Kind::brace_l)) {
            expect(Token::Kind::brace_l);
            sema_.pushScope();
            recBlock();
            sema_.popScope();
            expect(Token::Kind::brace_r);
        }
        else if(have(Token::Kind::kw_if)) {
            recIfElse();
        } else {
            compilerError("expected a code block or 'if'");
        }
        codegen_.closeIf();
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
    
    void Compiler::recFuncDecl() {
        expect(Token::Kind::kw_func);
        Token name = current();
        expect(Token::Kind::identifier);
        expect(Token::Kind::op_eq);
        std::vector<Sema::VarDecl> paramTypes;
        
        expect(Token::Kind::bracket_l);
        if(have(Token::Kind::identifier)) {
            paramTypes.push_back(recParamDecl());
            while(match(Token::Kind::comma))
                paramTypes.push_back(recParamDecl());
        }
        
        expect(Token::Kind::bracket_r);
        expect(Token::Kind::colon);
        auto returnType = recTypeDecl();
        expect(Token::Kind::brace_l);
        
        
        sema_.declareFunction(name, paramTypes, returnType);
        codegen_.openFunction(name, paramTypes.size());
        for(const auto& decl: paramTypes)
            codegen_.declareLocal(decl.first);
        
        recBlock();
        sema_.popScope();
        expect(Token::Kind::brace_r);
        codegen_.closeFunction();
    }

    Sema::VarDecl Compiler::recParamDecl() {
        Token name = current();
        expect(Token::Kind::identifier);
        expect(Token::Kind::colon);
        return std::make_pair(name, recTypeDecl());
    }
    
    Type Compiler::recTypeDecl() {
        if(match(Token::Kind::ty_integer))
            return Type::Integer;
        if(match(Token::Kind::ty_real))
            return Type::Number;
        if(match(Token::Kind::ty_string))
            return Type::String;
        if(match(Token::Kind::ty_bool))
            return Type::Bool;
        if(match(Token::Kind::ty_void))
            return Type::Void;
        compilerError("Invalid type name");
        return Type::Invalid;
    }
    
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
        else if(have(Token::Kind::kw_return)) {
            expect(Token::Kind::kw_return);
            // TODO: check return type with Sema
            if(haveTerm()) {
                recExpression(0);
                codegen_.emitInstruction(Opcode::ret_v);
            } else {
                codegen_.emitInstruction(Opcode::ret);
            }
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
        else if(have(Token::Kind::bracket_l)) {
            type = recFuncCall();
        }
        else if(match(Token::Kind::identifier)) {
            type = sema_.getVarType(symbol);
            codegen_.emitLocal(Opcode::load, symbol);
        }
        else if(match(Token::Kind::kw_yes)) {
            type = Type::Bool;
            codegen_.emitInstruction(Opcode::load_yes);
        }
        else if(match(Token::Kind::kw_no)) {
            type = Type::Bool;
            codegen_.emitInstruction(Opcode::load_no);
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
    
    TypeExpr Compiler::recFuncCall() {
        expect(Token::Kind::bracket_l);
        Token symA, symB;
        bool foreign = false;
        
        std::uint8_t arity = 0;
        
        symA = current();
        expect(Token::Kind::identifier);
        
        if(match(Token::Kind::op_dot)) {
            symB = current();
            expect(Token::Kind::identifier);
            foreign = true;
        }
        
        if(haveTerm()) {
            arity = 1;
            recExpression(0);
            while(match(Token::Kind::comma)) {
                arity += 1;
                recExpression(0);
            }
        }
        expect(Token::Kind::bracket_r);
        
        TypeExpr type{Type::Invalid};
        if(foreign) {
            type = sema_.getFuncType(symA, symB, arity);
            codegen_.emitConstantS(Opcode::call_f, VM::mangleFunc(manager_.tokenAsString(symA), manager_.tokenAsString(symB), arity));
        } else {
            type = sema_.getFuncType(symA, arity);
            codegen_.emitConstantS(Opcode::call_n, VM::mangleFunc(manager_.tokenAsString(symA), arity));
        }
        return type;
    }
}
