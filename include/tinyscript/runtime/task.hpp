//
//  task.hpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

#pragma once
#include <cstdint>
#include <iostream>
#include <string>

#include <tinyscript/opcodes.hpp>
#include <tinyscript/runtime/program.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    class Task {
    public:
        friend class VM;
        
        Task(const Program& program, std::uint32_t stackSize);
        ~Task();
        
        // MARK: - Stack Management
        
        void push(const Value& value);
        Value& pop();
        Value& peek() const;
        
        // MARK: - Program reads
        
        Opcode next();
        std::uint8_t read8();
        std::uint16_t read16();
        std::uint32_t stackSize() const;
        const Value& constant(std::uint8_t idx);
        
        // MARK: - Variables
        
        void load();
        void store();
        
        // MARK: - Stack Management
        
        void pushFrame(const Program::Function& func);
        void pushFrame(const std::string& name);
        void popFrame();
        
    private:
        struct Frame {
            const Program::Function&    function;
            std::uint64_t               callerIP;
            Value*                      base;
            Value*                      stack;
        };
        
        const Program&      program_;
        const std::uint32_t stackSize_;
        
        Value*              stack_;
        Value*              sp_;
        std::vector<Frame>  callStack_;
        std::uint64_t       ip_ = 0;
    };
    
    inline void Task::push(const Value& value) {
        assert(sp_+1 < stack_ + stackSize_ && "Coroutine stack overflow");
        *(sp_++) = value;
    }
    
    inline Value& Task::pop() {
        assert(sp_-1 >= stack_ && "Coroutine stack underflow");
        return *(--sp_);
    }
    
    inline Value& Task::peek() const {
        return *(sp_-1);
    }
    
    inline Opcode Task::next() {
        assert(callStack_.size() > 0 && "No function on call stack");
        return static_cast<Opcode>(callStack_.back().function.bytecode[ip_++]);
    }
    
    inline std::uint8_t Task::read8() {
        assert(callStack_.size() > 0 && "No function on call stack");
        const auto& bytecode = callStack_.back().function.bytecode;
        return bytecode[ip_++];
    }
    
    inline std::uint16_t Task::read16() {
        assert(callStack_.size() > 0 && "No function on call stack");
        ip_ += 2;
        const auto& bytecode = callStack_.back().function.bytecode;
        return (bytecode[ip_-2] << 8) | (bytecode[ip_-1]);
    }
    
    inline std::uint32_t Task::stackSize() const {
        return static_cast<std::uint32_t>(sp_ - stack_);
    }
    
    inline const Value& Task::constant(std::uint8_t idx) {
        return program_.constants[idx];
    }
    
    inline void Task::load() {
        assert(callStack_.size() > 0 && "No function on call stack");
        push(callStack_.back().base[read8()]);
    }
    
    inline void Task::store() {
        assert(callStack_.size() > 0 && "No function on call stack");
        callStack_.back().base[read8()] = pop();
    }
}
