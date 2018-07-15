//
//  TypeExpr.hpp
//  tinyscript
//
//  Created by Amy Parent on 10/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cassert>
#include <cstdint>

#include <tinyscript/type.hpp>
#include <tinyscript/compiler/token.hpp>

namespace tinyscript {
    
    class TypeExpr {
    public:
        
        TypeExpr(Type type) : type_(type), isLvalue_(false) {}
        TypeExpr(Type type, const Token& var) : type_(type), isLvalue_(true), variable_(var) {}
        
        TypeExpr(const TypeExpr& other) : type_(other.type_), isLvalue_(other.isLvalue_), variable_(other.variable_) {}
        TypeExpr& operator=(const TypeExpr& other) {
            if(this != &other) {
                type_ = other.type_;
                isLvalue_ = other.isLvalue_;
                variable_ = other.variable_;
            }
            return *this;
        }
        
        void setLValue(const Token& var) { isLvalue_ = true; variable_ = var; }
        
        const Token& lvalue() const {
            assert(isLvalue_ && "type isn't an l-value");
            return variable_;
        }
        
        bool is(Type type) const { return type_ == type; }
        Type unqualifiedType() const { return type_; }
        bool isValid() const { return type_ != Type::Invalid; }
        bool isLValue() const { return isLvalue_; }
        
    private:
        Type type_;
        bool isLvalue_;
        Token variable_;
    };
    
    static inline bool qualifiedEq(const TypeExpr& a, const TypeExpr& b) {
        return a.unqualifiedType() == b.unqualifiedType() && a.isLValue() == b.isLValue();
    }
    
    static inline bool unqualifiedEq(const TypeExpr& a, const TypeExpr& b) {
        return a.unqualifiedType() == b.unqualifiedType();
    }
    
    static inline Type constrain(const TypeExpr& a, const TypeExpr& b, bool strict = false) {
        if(unqualifiedEq(a, b)) return a.unqualifiedType();
        auto typeMask = a.unqualifiedType() | b.unqualifiedType();
        if(typeMask == (Type::Number | Type::Integer)) return Type::Number;
        if((typeMask & Type::String) && !strict) return Type::String;
        return Type::Invalid;
    }
}
