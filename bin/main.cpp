//
//  main.cpp
//  mvm
//
//  Created by Amy Parent on 02/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>

#include <tinyscript/compiler/compiler.hpp>
#include <tinyscript/compiler/sourcemanager.hpp>

#include <tinyscript/runtime/vm.hpp>
#include <tinyscript/runtime/program.hpp>
#include <tinyscript/runtime/library.hpp>
#include <tinyscript/runtime/task.hpp>


using namespace tinyscript;
int main(int argc, const char * argv[]) {
    
    tinyscript::VM vm;
    tinyscript::StdLib lib;
    
    vm.registerModule(lib.system());
    vm.registerModule(lib.io());
    vm.registerModule(lib.random());
    vm.registerModule(lib.string());
    vm.registerModule(lib.reflection());

    if(argc != 2) {
        std::cerr << "error: wrong number of arguments" << std::endl;
        std::cerr << "usage: " << argv[0] << " script_file" << std::endl;
        return -1;
    }
    
    std::ifstream input(argv[1]);
    if(!input.is_open()) {
        std::cerr << "error: cannot open code file '" << argv[1] << "'" << std::endl;
        return -1;
    }
    
    SourceManager manager{input};
    Compiler comp{vm, manager};
    auto prog = comp.compile();
    
    Task task{prog, 256};
    auto result = vm.run(task);
    while(result.first == VM::Result::Continue) {
        std::cout << "yield: " << result.second.repr() << std::endl;
        result = vm.run(task);
    }
    if(result.first == VM::Result::Error) {
        std::cerr << "runtime error: " << result.second.asString() << std::endl;
        return -1;
    }
    
    return 0;
}
