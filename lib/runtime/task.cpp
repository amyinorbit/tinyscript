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
        locals_ = stack_ + program.variableCount;
        sp_ = locals_;
    }
    
    Task::~Task() {}
}
