//
//  task.cpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <cassert>
#include <tinyscript/runtime/task.hpp>

#include <tinyscript/runtime/program.hpp>

namespace tinyscript {
    
    Task::Task(const Program& program, std::uint32_t stackSize)
    : program_(program)
    , stackSize_(stackSize) {
        
        stack_ = new Value[stackSize];
        sp_ = stack_;
        pushFrame(program_.script);
    }

    Task::Task(const Program& program, Task* caller, const std::string& function)
    : program_(program)
    , stackSize_(caller->stackSize_)
    , caller_(caller) {
        stack_ = new Value[stackSize_];
        sp_ = stack_;
        pushFrame(function);
    }
    
    Task::~Task() {}
    
    void Task::pushFrame(const Program::Function& func) {
        auto callerIP = ip_;
        auto* base = sp_ - func.arity;
        auto* stack = base + func.variableCount;
        ip_ = 0;
        sp_ = stack;
        callStack_.push_back(Frame{func, callerIP, base, stack});
    }
    
    void Task::pushFrame(const std::string& name) {
        const auto& it = program_.functions.find(name);
        assert(it != program_.functions.end() && "Invalid symbolic reference");
        pushFrame(it->second);
    }
    
    bool Task::popFrame() {
        assert(callStack_.size() > 0 && "Call stack underflow");
        ip_ = callStack_.back().callerIP;
        sp_ = callStack_.back().base;
        callStack_.pop_back();
        return callStack_.empty();
    }
    
    bool Task::returnFrame() {
        assert(callStack_.size() > 0 && "Call stack underflow");
        Value ret = pop();
        ip_ = callStack_.back().callerIP;
        sp_ = callStack_.back().base;
        callStack_.pop_back();
        push(ret);
        return callStack_.empty();
    }
}
