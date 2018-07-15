//
//  sema.cpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <iostream>
#include <cassert>
#include <tinyscript/compiler/sema.hpp>
#include <tinyscript/compiler/sourcemanager.hpp>
#include <tinyscript/runtime/vm.hpp>


namespace tinyscript {
    
    Sema::Sema(const VM& vm, const SourceManager& manager)
    : vm_(vm)
    , manager_(manager) {
        // Global scope
        Scope global;
        scopes_.push_back(global);
        
    }
    
    void Sema::pushScope() {
        Scope newScope;
        scopes_.push_back(newScope);
    }
    
    void Sema::popScope() {
        assert(scopes_.size() > 1 && "scope underflow");
        scopes_.pop_back();
    }
    
    bool Sema::declareVariable(const Token& symbol, Type type) {
        auto key = manager_.tokenAsString(symbol);
        auto& scope = scopes_.back();
        auto it = scope.variables.find(key);
        
        if(it != scope.variables.end()) {
            semanticError(symbol, "variable '" + key + "' already declared in this scope");
            semanticNote(it->second.declLocation, "first declaration was here");
            return false;
        }
        
        scope.variables[key].declLocation = symbol;
        scope.variables[key].type = type;
        return true;
    }
    
    TypeExpr Sema::getVarType(const Token& symbol) {
        auto key = manager_.tokenAsString(symbol);
        
        for(std::int64_t i = scopes_.size()-1; i >= 0; --i) {
            auto& scope = scopes_[i];
            auto it = scope.variables.find(key);
            if(it != scope.variables.end()) return TypeExpr(it->second.type, symbol);
        }
        
        semanticError(symbol, "unkown variable '" + key + "'");
        return TypeExpr(Type::Invalid, symbol);
    }
    
    TypeExpr Sema::getFuncType(const Token& module, const Token& symbol, std::uint8_t arity) {
        auto key1 = manager_.tokenAsString(module);
        auto key2 = manager_.tokenAsString(symbol);
        auto type = vm_.functionType(key1, key2, arity);
        if(type == Type::Invalid) {
            semanticError(symbol, "unkown function '" + key1 + "." + key2 + "'");
        }
        return type;
    }
    
    Sema::OperatorMapping Sema::binaryOpType(const Token& op, TypeExpr lhs, TypeExpr rhs) {
        assert(op.isBinaryOp() && "token is not an operator");
        if(!lhs.isValid() && !rhs.isValid()) return {Type::Invalid, Type::Invalid};
        if(lhs.is(Type::Void) || rhs.is(Type::Void)) {
            semanticNote(op, "cannot operate on void types");
            return {Type::Invalid, Type::Invalid};
        }
        
        if(!lhs.isValid()) lhs = rhs;
        if(!rhs.isValid()) rhs = lhs;
        
        switch(op.operatorType()) {
            case Token::OperatorType::Arithmetic:
            {
                auto constrainedType = constrain(lhs, rhs);
                if(constrainedType == Type::Invalid) {
                    semanticError(op, "arithmetic expression type mismatch");
                    return {Type::Invalid, Type::Invalid};
                }
                return {constrainedType, constrainedType};
            }
                
            case Token::OperatorType::String:
                if(!lhs.is(Type::String) || !rhs.is(Type::String)) {
                    semanticError(op, "both sides of string operators must be strings");
                    return {Type::Invalid, Type::Invalid};
                }
                return {Type::String, Type::String};
                
            case Token::OperatorType::Relational:
            {
                auto constrainedType = constrain(lhs, rhs, true);
                if(constrainedType == Type::Invalid) {
                    semanticError(op, "relational expression type mismatch");
                    return {Type::Invalid, Type::Invalid};
                }
                return {constrainedType, Type::Bool};
            }
                
            case Token::OperatorType::Logical:
                if(!lhs.is(Type::Bool) || !rhs.is(Type::Bool)) {
                    semanticError(op, "both sides of logical operators must be boolean expressions");
                    return {Type::Invalid, Type::Invalid};
                }
                return {Type::Bool, Type::Bool};
                
            case Token::OperatorType::Assignment:
            {
                auto constrainedType = constrain(lhs, rhs, true);
                if(constrainedType == Type::Invalid) {
                    semanticError(op, "relational expression type mismatch");
                    return {Type::Invalid, Type::Invalid};
                }
                return {lhs.unqualifiedType(), lhs.unqualifiedType()};
            }
        }
    }
    
    void Sema::semanticError(const Token& symbol, const std::string& message) const {
        std::cerr << "error: " << message << std::endl;
        manager_.printLineAndToken(std::cerr, symbol.location, symbol.length);
        std::cerr << std::endl;
    }
    
    void Sema::semanticNote(const Token& symbol, const std::string& message) const {
        std::cerr << "note: " << message << std::endl;
        manager_.printLineAndToken(std::cerr, symbol.location, symbol.length);
        std::cerr << std::endl;
    }
}
