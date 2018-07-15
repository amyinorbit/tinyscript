//
//  value.hpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <string>
#include <memory>

namespace tinyscript {
    struct Value {
        enum class Kind { Nil, Bool, Int, Number, String };
        Kind kind;
        
        Value() : kind(Kind::Nil) {}
        explicit Value(const std::string& value) : kind(Kind::String) {
            new (&stringValue) std::string{value};
        }
        
        static Value Float(double value);
        static Value Integer(std::int64_t value);
        static Value boolean(bool value);
        
        Value(const Value& other) : kind(other.kind) {
            switch(kind) {
                case Kind::String: new (&stringValue) std::string{other.stringValue}; break;
                case Kind::Bool: boolValue = other.boolValue; break;
                case Kind::Int: intValue = other.intValue; break;
                case Kind::Number: floatValue = other.floatValue; break;
                default: break;
            }
        }
        
        Value& operator=(const Value& other) {
            if(this != &other) {
                destroy();
                kind = other.kind;
                
                switch(kind) {
                    case Kind::String: new (&stringValue) std::string{other.stringValue}; break;
                    case Kind::Bool: boolValue = other.boolValue; break;
                    case Kind::Int: intValue = other.intValue; break;
                    case Kind::Number: floatValue = other.floatValue; break;
                    default: break;
                }
            }
            return *this;
        }
        
        ~Value() { destroy(); }
        
        bool asBool() const;
        double asNumber() const;
        std::int64_t asInt() const;
        const std::string& asString() const;
        std::string repr() const;
    private:
        
        void destroy() {
            using std::string;
            if(kind == Kind::String) {
                (&stringValue)->string::~string();
            }
        }
        
        union {
            std::string stringValue;
            bool boolValue;
            std::int64_t intValue;
            double floatValue;
        };
    };
    
    inline Value Value::Integer(std::int64_t value) {
        static Value v;
        v.kind = Kind::Int;
        v.intValue = value;
        return v;
    }
    
    inline Value Value::Float(double value) {
        static Value v;
        v.kind = Kind::Number;
        v.floatValue = value;
        return v;
    }
    
    inline Value Value::boolean(bool value) {
        static Value v;
        v.kind = Kind::Bool;
        v.boolValue = value;
        return v;
    }
    
    inline bool Value::asBool() const {
        return kind == Kind::Bool && boolValue;
    }
    
    inline std::int64_t Value::asInt() const {
        return kind == Kind::Int ? intValue : 0;
    }
    
    inline double Value::asNumber() const {
        return kind == Kind::Number ? floatValue : 0.f;
    }
    
    inline const std::string& Value::asString() const {
        return stringValue;
    }
    
    inline std::string Value::repr() const {
        switch (kind) {
            case Kind::Nil: return "<nil>";
            case Value::Kind::Bool: return boolValue ? "true" : "false";
            case Value::Kind::String: return stringValue;
            case Value::Kind::Int: return std::to_string(intValue);
            case Value::Kind::Number: return std::to_string(floatValue);
        }
    }
    
    inline bool operator==(const Value& a, const Value& b) {
        if(a.kind != b.kind) return false;
        switch(a.kind) {
            case Value::Kind::Nil:
                return false; // Nil values are never equal
            case Value::Kind::Bool:
                return a.asBool() == b.asBool();
            case Value::Kind::Int:
                return a.asInt() == b.asInt();
            case Value::Kind::Number:
                return a.asNumber() == b.asNumber();
            case Value::Kind::String:
                return a.asString() == b.asString();
        }
    }
}

