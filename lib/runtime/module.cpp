//
//  Module.cpp
//  tinyscript
//
//  Created by Amy Parent on 09/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <tinyscript/runtime/module.hpp>
#include <tinyscript/runtime/vm.hpp>

namespace tinyscript {
    void Module::addFunction(const std::string& symbol, uint8_t arity, Type returnType, VM::Foreign func) {
        auto name = VM::mangleFunc(name_, symbol, arity);
        assert(functions_.find(name) == functions_.end() && "function is already decalred");
        functions_[name] = VM::Function{name, arity, returnType, func};
    }
    
    void Module::addVariable(const std::string& symbol, const Value& value) {
        auto name = VM::mangleVar(name_, symbol);
        assert(variables_.find(name) == variables_.end() && "function is already decalred");
        variables_[name] = value;
    }
}
