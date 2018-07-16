//
//  minivm.cpp
//  mvm
//
//  Created by Amy Parent on 02/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <algorithm>
#include <tinyscript/runtime/vm.hpp>

#include <tinyscript/opcodes.hpp>
#include <tinyscript/runtime/program.hpp>
#include <tinyscript/runtime/task.hpp>
#include <tinyscript/runtime/module.hpp>


namespace tinyscript {
    VM::VM() {
        
    }
    
    VM::~VM() {
        
    }
    
    std::string VM::mangleFunc(const std::string& module, const std::string& symbol, uint8_t arity) {
        return "_F" + std::to_string(module.size()) + module + std::to_string(symbol.size()) + symbol + "_$" + std::to_string(arity);
    }
    
    std::string VM::mangleVar(const std::string& module, const std::string& symbol) {
        return "_V" + std::to_string(module.size()) + module + std::to_string(symbol.size()) + symbol;
    }
    
    void VM::registerModule(const tinyscript::Module &module) {
        //modules_[module.name] = module;
        for(const auto& pair: module.functions()) {
            functions_[pair.first] = pair.second;
        }
    }
    
    Type VM::functionType(const std::string& module, const std::string& symbol, std::uint8_t arity) const {
        auto sig = mangleFunc(module, symbol, arity);
        auto it = functions_.find(sig);
        if(it == functions_.end()) return Type::Invalid;
        return it->second.returnType;
    }
    

    bool VM::functionExists(const std::string& module, const std::string& symbol, std::uint8_t arity) const {
        auto sig = mangleFunc(module, symbol, arity);
        auto it = functions_.find(sig);
        return it != functions_.end();
    }
    
    // bool VM::moduleExists(const std::string& module) const {
    //     return modules_.find(module) != modules_.end();
    // }
    
    std::pair<VM::Result, Value> VM::run(tinyscript::Task &co) {
        for(;;) {
            auto instr = co.next();
            
#ifdef DEBUG_VMSTACK
            std::cout << "[dbg] inst: " << instr << std::endl;
            if(co.stackSize() > 0)
                std::cout << "      tos[" << co.stackSize() << "]: " << co.peek().repr() << std::endl;
            else
                std::cout << "      [no stack]" << std::endl;
#endif
            switch (instr) {
                case Opcode::halt:
                    return std::make_pair(Result::Done, Value());
                    
                case Opcode::load_c:
                    co.push(co.constant(co.read8()));
                    break;
                    
                case Opcode::load:
                    co.load();
                    break;
                    
                case Opcode::store:
                    co.store();
                    break;
                    
                case Opcode::fadd:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::Float(a + b));
                }
                    break;
                    
                case Opcode::fsub:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::Float(a - b));
                }
                    break;
                    
                case Opcode::fmul:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::Float(a * b));
                }
                    break;
                    
                case Opcode::fdiv:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::Float(a / b));
                }
                    break;
                    
                case Opcode::fmin:
                {
                    double a = co.pop().asNumber();
                    co.push(Value::Float(-a));
                }
                    break;
                    
                case Opcode::i2f:
                    co.push(Value::Float(static_cast<double>(co.pop().asNumber())));
                    break;
                    
                case Opcode::f2i:
                    co.push(Value::Integer(static_cast<double>(co.pop().asInt())));
                    break;
                    
                case Opcode::iadd:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::Integer(a + b));
                }
                    break;
                    
                case Opcode::isub:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::Integer(a - b));
                }
                    break;
                    
                case Opcode::imul:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::Integer(a * b));
                }
                    break;
                    
                case Opcode::idiv:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::Integer(a / b));
                }
                    break;
                    
                case Opcode::imin:
                {
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::Integer(-a));
                }
                    break;
                    
                case Opcode::sadd:
                {
                    const auto& b = co.pop().asString();
                    const auto& a = co.pop().asString();
                    co.push(Value(a + b));
                }
                    break;
                    
                case Opcode::log_and:
                {
                    const auto& b = co.pop().asBool();
                    const auto& a = co.pop().asBool();
                    co.push(Value::boolean(a && b));
                }
                    break;
                    
                case Opcode::log_or:
                {
                    const auto& b = co.pop().asBool();
                    const auto& a = co.pop().asBool();
                    co.push(Value::boolean(a || b));
                }
                    break;
                    
                case Opcode::test_flt:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::boolean(a < b));
                }
                    break;
                
                case Opcode::test_flteq:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::boolean(a <= b));
                }
                    break;
                    
                case Opcode::test_fgt:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::boolean(a > b));
                }
                    break;
                    
                case Opcode::test_fgteq:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::boolean(a >= b));
                }
                    break;
                    
                case Opcode::test_feq:
                {
                    double b = co.pop().asNumber();
                    double a = co.pop().asNumber();
                    co.push(Value::boolean(a == b));
                }
                    break;
                    
                case Opcode::test_ilt:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::boolean(a < b));
                }
                    break;
                    
                case Opcode::test_ilteq:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::boolean(a <= b));
                }
                    break;
                    
                case Opcode::test_igt:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::boolean(a > b));
                }
                    break;
        
                case Opcode::test_igteq:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::boolean(a >= b));
                }
                    break;
                    
                case Opcode::test_ieq:
                {
                    std::int64_t b = co.pop().asInt();
                    std::int64_t a = co.pop().asInt();
                    co.push(Value::boolean(a == b));
                }
                    break;
                    
                case Opcode::jmp:
                    co.ip_ += co.read16();
                    break;
                    
                case Opcode::rjmp:
                    co.ip_ -= co.read16();
                    break;
                    
                case Opcode::jnz:
                    if(co.pop().asBool()) {
                        co.ip_ += co.read16();
                    } else {
                        co.ip_ += 2;
                    }
                    break;
                    
                case Opcode::rjnz:
                    if(co.pop().asBool()) {
                        co.ip_ -= co.read16();
                    } else {
                        co.ip_ += 2;
                    }
                    break;
                    
                case Opcode::retain:
                    // TODO: implement objects?
                    break;
                    
                case Opcode::release:
                    // TODO: implement objects?
                    break;
                    
                case Opcode::call_f:
                {
                    const auto& signature = co.constant(co.read8());
                    functions_.at(signature.asString()).code(*this, co);
                }
                    break;
                    
                case Opcode::yield:
                    return std::make_pair(Result::Continue, Value());
                    break;
                    
                case Opcode::yield_v:
                    return std::make_pair(Result::Continue, co.pop());
                    break;
                    
                case Opcode::fail:
                    return std::make_pair(Result::Error, co.constant(co.read8()));
                    break;
                    
                case Opcode::nop:
                    break;
            }
        }
        return std::make_pair(Result::Done, Value());
    }
}

