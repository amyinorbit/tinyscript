//
//  library.cpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <cstdlib>
#include <tinyscript/runtime/library.hpp>

#include <tinyscript/runtime/vm.hpp>
#include <tinyscript/runtime/task.hpp>

namespace tinyscript {
    
    StdLib::StdLib() : system_("System"), io_("IO"), string_("String"), random_("Random") {
        system_.addFunction("GetOS", 0, Type::String, [](VM& vm, Task& co) {
            co.push(Value(std::string("macOS")));
        });
        
        system_.addFunction("GetTime", 0, Type::Number, [](VM& vm, Task& co) {
            co.push(Value::Integer(time(nullptr)));
        });
        
        io_.addFunction("Print", 1, Type::Void, [](VM& vm, Task& co) {
            const auto& v = co.pop();
            std::cout << v.repr() << std::endl;
        });
        
        io_.addFunction("ToString", 1, Type::String, [](VM& vm, Task& co) {
            co.push(Value(co.pop().repr()));
        });
        
        random_.addFunction("Float", 2, Type::Number, [](VM& vm, Task& co) {
            auto high = co.pop().asNumber();
            auto low = co.pop().asNumber();
            
            double m = high-low;
            double r = low + (m * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));
            co.push(Value::Float(r));
        });
        
        random_.addFunction("Float", 1, Type::Number, [](VM& vm, Task& co) {
            auto m = co.pop().asNumber();
            double r = m * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
            co.push(Value::Float(r));
        });
        
        random_.addFunction("Integer", 2, Type::Integer, [](VM& vm, Task& co) {
            auto high = co.pop().asInt();
            auto low = co.pop().asInt();
            
            std::int64_t m = high-low;
            co.push(Value::Integer(low + (static_cast<std::uint64_t>(std::rand()) % m)));
        });
        
        random_.addFunction("Integer", 1, Type::Integer, [](VM& vm, Task& co) {
            auto m = co.pop().asInt();
            co.push(Value::Integer(static_cast<std::uint64_t>(std::rand()) % m));
        });
        
        random_.addFunction("Seed", 1, Type::Void, [](VM& vm, Task& co) {
            std::srand(static_cast<unsigned int>(co.pop().asInt()));
        });
        
        string_.addFunction("Equal", 2, Type::Bool, [](VM& vm, Task& co) {
            const auto& b = co.pop().asString();
            const auto& a = co.pop().asString();
            co.push(Value::boolean(a == b));
        });
        
        string_.addFunction("Slice", 3, Type::String, [](VM& vm, Task& co) {
            const auto& length = co.pop().asInt();
            const auto& begin = co.pop().asInt();
            const auto& str = co.pop().asString();
            co.push(Value(str.substr(begin, length)));
        });
        
//        string_.addFunction("ToUpper", 1, Type::String, [](VM& vm, Task& co) {
//            const auto& str = co.pop().asString();
//            co.push(Value(str.upp));
//        });
    }
}
