//
//  sema.hpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <tinyscript/type.hpp>
#include <tinyscript/compiler/token.hpp>
#include <tinyscript/compiler/typeexpr.hpp>

namespace tinyscript {
    class VM;
    class SourceManager;
    
    class Sema {
    public:
        struct OperatorMapping {Type from; Type to;};
        Sema(const VM& vm, const SourceManager& manager);
        
        using VarDecl = std::pair<Token, Type>;
        
        void pushScope();
        void popScope();
        
        bool declareFunction(const Token& name, const std::vector<VarDecl>& paramTypes, Type returnType);
        bool declareVariable(const Token& symbol, Type type);
        
        TypeExpr getVarType(const Token& symbol);
        TypeExpr getFuncType(const Token& symbol, std::uint8_t arity);
        TypeExpr getFuncType(const Token& module, const Token& symbol, std::uint8_t arity);
        
        OperatorMapping binaryOpType(const Token& op, TypeExpr lhs, TypeExpr rhs);
        void semanticError(const Token& symbol, const std::string& message) const;
        void semanticNote(const Token& symbol, const std::string& message) const;
        
    private:
        struct Var {
            Type type;
            Token declLocation;
        };
        
        struct Func {
            std::vector<Type>   paramTypes;
            Type                returnType;
            Token               declLocation;
        };
        
        struct Scope {
            std::map<std::string, Func> functions;
            std::map<std::string, Var>  variables;
        };
        
        const VM& vm_;
        const SourceManager& manager_;
        std::vector<Scope> scopes_;
    };
}
