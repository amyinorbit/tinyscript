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
#include <map>
#include <cstdint>
#include <utility>

#include <tinyscript/type.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    class Module;
    class Task;
    class Program;
    
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
        
        using DispatchTable = std::map<std::string, Function>;
        
        VM();
        ~VM();
        
        static std::string mangleFunc(const std::string& module, const std::string& symbol, uint8_t arity);
        static std::string mangleVar(const std::string& module, const std::string& symbol);
        
        //void addFunction(const std::string& symbol, uint8_t arity, Type returnType, Foreign func);
        void registerModule(const Module& module);
        Type functionType(const std::string& module, const std::string& symbol, std::uint8_t arity) const;
        std::pair<Result, Value> run(Task& co);
        
    private:
        DispatchTable functions_;
    };
}



