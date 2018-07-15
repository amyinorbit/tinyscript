//
//  codegen.hpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <string>
#include <tinyscript/opcodes.hpp>
#include <tinyscript/compiler/ilbuilder.hpp>
#include <tinyscript/runtime/program.hpp>

namespace tinyscript {
    struct Token;
    class SourceManager;
    
    class CodeGen {
    public:
        
        CodeGen(const SourceManager& manager);
        
        void openIf();
        void closeIf();
        
        std::string ifLabel() const;
        std::string elseLabel() const;
        std::string endifLabel() const;
        
        void openLoop();
        void closeLoop();
        
        std::string loopLabel() const;
        std::string endLoopLabel() const;
        std::string loopVariable() const;
        
        ILBuilder::InsertLocation patchPoint();
        void patchConversion(Opcode code, ILBuilder::InsertLocation at);
        void patchCall(Opcode code, const std::string& symbol, ILBuilder::InsertLocation at);
        void dropCode(ILBuilder::InsertLocation at);
        
        void declareLocal(const Token& symbol);
        
        void emitLabel(const std::string& label);
        
        void emitLocal(Opcode code, const Token& symbol);
        void emitLocal(Opcode code, const std::string& symbol);
        
        void emitConstantI(Opcode code, const Token& symbol);
        void emitConstantI(Opcode code, std::int64_t symbol);
        
        void emitConstantF(Opcode code, const Token& symbol);
        void emitConstantF(Opcode code, float symbol);
        
        void emitConstantS(Opcode code, const Token& symbol);
        void emitConstantS(Opcode code, const std::string& symbol);
        
        void emitInstruction(Opcode code);
        void emitJump(Opcode code, const std::string& label);
        
        Program generate(bool dump);
        
    private:
        std::uint64_t               ifID_       = 0;
        std::uint64_t               loopID_     = 0;
        std::vector<std::uint64_t>  ifStack_;
        std::vector<std::uint64_t>  loopStack_;
        
        ILBuilder                   builder_;
        const SourceManager&        manager_;
    };
}
