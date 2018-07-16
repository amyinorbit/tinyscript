//
//  library.hpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <tinyscript/runtime/module.hpp>

namespace tinyscript {
    class VM;
    
    class StdLib {
    public:
        StdLib();
        
        const Module& system() const { return system_; }
        const Module& io() const { return io_; }
        const Module& string() const { return string_; }
        const Module& random() const { return random_; }
        const Module& reflection() const { return reflection_; }
        
    private:
        Module random_;
        Module system_;
        Module io_;
        Module string_;
        Module reflection_;
    };
}
