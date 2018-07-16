//
//  minivm.hpp
//  mvm
//
//  Created by Amy Parent on 02/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <utility>

#include <tinyscript/type.hpp>
//#include <tinyscript/runtime/module.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    class Task;
    class Program;
    class Module;
    
    class VM {
    public:
        enum class Result {Done, Continue, Error};
        using Foreign = std::function<void(VM&, Task&)>;
        
        struct Function {
            std::string     symbol;
            std::uint8_t    arity;
            Type            returnType;
            Foreign         code;
        };
        
        using DispatchTable = std::unordered_map<std::string, Function>;
        using ModuleTable = std::unordered_map<std::string, Module>;
        
        VM();
        ~VM();
        
        static std::string mangleFunc(const std::string& module, const std::string& symbol, uint8_t arity);
        static std::string mangleVar(const std::string& module, const std::string& symbol);
        
        //void addFunction(const std::string& symbol, uint8_t arity, Type returnType, Foreign func);
        void registerModule(const Module& module);
        Type functionType(const std::string& module, const std::string& symbol, std::uint8_t arity) const;
        
        bool functionExists(const std::string& module, const std::string& symbol, std::uint8_t arity) const;
        //bool moduleExists(const std::string& module) const;
        
        std::pair<Result, Value> run(Task& co);
        
    private:
        //ModuleTable modules_;
        DispatchTable functions_;
    };
}



