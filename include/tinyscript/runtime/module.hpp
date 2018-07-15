//
//  Module.hpp
//  tinyscript
//
//  Created by Amy Parent on 09/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <tinyscript/type.hpp>
#include <tinyscript/runtime/vm.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {

    class Task;
    
    class Module {
    public:
        
        using FunctionTable = std::unordered_map<std::string, VM::Function>;
        using VariableTable = std::unordered_map<std::string, Value>;
        
        Module(const std::string& name) : name_(name) {}
        
        void addFunction(const std::string& symbol, uint8_t arity, Type returnType, VM::Foreign func);
        void addVariable(const std::string& symbol, const Value& value);
        
        const std::string& name() const { return name_; }
        const FunctionTable& functions() const { return functions_; }
        const VariableTable& variables() const { return variables_; }
        
    private:
        std::string name_;
        FunctionTable functions_;
        VariableTable variables_;
    };
}
