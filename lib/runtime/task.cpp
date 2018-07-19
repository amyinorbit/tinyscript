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
    
    Task::~Task() {}
    
    void Task::pushFrame(const Program::Function& func) {
        auto callerIP = ip_;
        auto* base = sp_;
        auto* stack = sp_ + func.variableCount;
        ip_ = 0;
        callStack_.push_back(Frame{func, callerIP, base, stack});
    }
    
    void Task::pushFrame(const std::string& name) {
        const auto& it = program_.functions.find(name);
        assert(it != program_.functions.end() && "Invalid symbolic reference");
        pushFrame(it->second);
    }
    
    void Task::popFrame() {
        assert(callStack_.size() > 0 && "Call stack underflow");
        ip_ = callStack_.back().callerIP;
        sp_ = callStack_.back().base;
        callStack_.pop_back();
    }
}
