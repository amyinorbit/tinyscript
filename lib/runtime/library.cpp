//
//  library.cpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <cstdlib>
#include <string>
#include <tinyscript/runtime/library.hpp>

#include <tinyscript/runtime/vm.hpp>
#include <tinyscript/runtime/task.hpp>

namespace tinyscript {
    
    StdLib::StdLib()
    : random_("Random")
    , system_("System")
    , io_("IO")
    , string_("String")
    , reflection_("Reflection") {
        system_.addFunction("getOS", 0, Type::String, [](VM& vm, Task& co) {
            co.push(Value(std::string("macOS")));
        });
        
        system_.addFunction("getTime", 0, Type::Number, [](VM& vm, Task& co) {
            co.push(Value::Integer(time(nullptr)));
        });
        
        io_.addFunction("print", 1, Type::Void, [](VM& vm, Task& co) {
            const auto& v = co.pop();
            std::cout << v.repr() << std::endl;
        });
        
        io_.addFunction("toString", 1, Type::String, [](VM& vm, Task& co) {
            co.push(Value(co.pop().repr()));
        });
        
        io_.addFunction("getLine", 0, Type::String, [](VM& vm, Task& co) {
            std::string line;
            std::getline(std::cin, line);
            co.push(Value(line));
        });
        
        io_.addFunction("getLine", 1, Type::String, [](VM& vm, Task& co) {
            const auto& prompt = co.pop().asString();
            std::cout << prompt;
            std::string line;
            std::getline(std::cin, line);
            co.push(Value(line));
        });
        
        random_.addFunction("float", 2, Type::Number, [](VM& vm, Task& co) {
            auto high = co.pop().asNumber();
            auto low = co.pop().asNumber();
            
            double m = high-low;
            double r = low + (m * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));
            co.push(Value::Float(r));
        });
        
        random_.addFunction("float", 1, Type::Number, [](VM& vm, Task& co) {
            auto m = co.pop().asNumber();
            double r = m * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
            co.push(Value::Float(r));
        });
        
        random_.addFunction("integer", 2, Type::Integer, [](VM& vm, Task& co) {
            auto high = co.pop().asInt();
            auto low = co.pop().asInt();
            
            std::int64_t m = high-low;
            co.push(Value::Integer(low + (static_cast<std::uint64_t>(std::rand()) % m)));
        });
        
        random_.addFunction("integer", 1, Type::Integer, [](VM& vm, Task& co) {
            auto m = co.pop().asInt();
            co.push(Value::Integer(static_cast<std::uint64_t>(std::rand()) % m));
        });
        
        random_.addFunction("seed", 1, Type::Void, [](VM& vm, Task& co) {
            std::srand(static_cast<unsigned int>(co.pop().asInt()));
        });
        
        string_.addFunction("equal", 2, Type::Bool, [](VM& vm, Task& co) {
            const auto& b = co.pop().asString();
            const auto& a = co.pop().asString();
            co.push(Value::boolean(a == b));
        });
        
        string_.addFunction("slice", 3, Type::String, [](VM& vm, Task& co) {
            const auto& length = co.pop().asInt();
            const auto& begin = co.pop().asInt();
            const auto& str = co.pop().asString();
            co.push(Value(str.substr(begin, length)));
        });
        
        reflection_.addFunction("mangle", 1, Type::String, [](VM& vm, Task& co) {
            const auto& signature = co.pop().asString();
        });
        
        reflection_.addFunction("mangle", 3, Type::String, [](VM& vm, Task& co) {
            std::uint64_t arity = co.pop().asInt();
            const auto& func = co.pop().asString();
            const auto& module = co.pop().asString();
            co.push(Value(vm.mangleFunc(module, func, arity))); 
        });
        
        reflection_.addFunction("functionExists", 3, Type::Bool, [](VM& vm, Task& co) {
            std::uint64_t arity = co.pop().asInt();
            const auto& func = co.pop().asString();
            const auto& module = co.pop().asString();
            co.push(Value::boolean(vm.functionExists(module, func, arity))); 
        });
        
        // reflection_.addFunction("ModuleExists", 1, Type::Bool, [](VM& vm, Task& co){
        //     const auto& module = co.pop().asString();
        //     co.push(Value::boolean(vm.moduleExists(module)));
        // });
        
//        string_.addFunction("ToUpper", 1, Type::String, [](VM& vm, Task& co) {
//            const auto& str = co.pop().asString();
//            co.push(Value(str.upp));
//        });
    }
}
