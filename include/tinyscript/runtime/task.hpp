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

#include <tinyscript/opcodes.hpp>
#include <tinyscript/runtime/program.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    class Program;
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
        
    private:
        const Program&      program_;
        const std::uint32_t stackSize_;
        
        std::uint64_t       ip_ = 0;
        
        Value*              stack_;
        Value*              locals_;
        Value*              sp_;
    };
    
    inline void Task::push(const Value& value) {
        assert(sp_+1 < stack_ + stackSize_ && "Coroutine stack overflow");
        *(sp_++) = value;
    }
    
    inline Value& Task::pop() {
        assert(sp_-1 >= locals_ && "Coroutine stack underflow");
        return *(--sp_);
    }
    
    inline Value& Task::peek() const {
        return *(sp_-1);
    }
    
    inline Opcode Task::next() {
        return static_cast<Opcode>(program_.bytecode[ip_++]);
    }
    
    inline std::uint8_t Task::read8() {
        return program_.bytecode[ip_++];
    }
    
    inline std::uint16_t Task::read16() {
        ip_ += 2;
        return (program_.bytecode[ip_-2] << 8) | (program_.bytecode[ip_-1]);
    }
    
    inline std::uint32_t Task::stackSize() const {
        return static_cast<std::uint32_t>(sp_ - locals_);
    }
    
    inline const Value& Task::constant(std::uint8_t idx) {
        return program_.constants[idx];
    }
    
    inline void Task::load() {
        push(stack_[read8()]);
    }
    
    inline void Task::store() {
        stack_[read8()] = pop();
    }
}


