//
//  codegen
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <iostream>
#include <tinyscript/compiler/codegen.hpp>
#include <tinyscript/compiler/sourcemanager.hpp>
#include <tinyscript/compiler/token.hpp>

namespace tinyscript {
    
    CodeGen::CodeGen(const SourceManager& manager) : manager_(manager) {}
    
    void CodeGen::openIf() {
        ifStack_.push_back(ifID_++);
    }
    
    void CodeGen::closeIf() {
        builder_.addSymbol(endifLabel());
        ifStack_.pop_back();
    }
    
    std::string CodeGen::ifLabel() const { return "if_" + std::to_string(ifStack_.back()); }
    std::string CodeGen::elseLabel() const { return "else_" + std::to_string(ifStack_.back());}
    std::string CodeGen::endifLabel() const { return "endif_" + std::to_string(ifStack_.back());}
    
    void CodeGen::openLoop() {
        loopStack_.push_back(loopID_++);
    }
    
    void CodeGen::closeLoop() {
        builder_.addSymbol(endLoopLabel());
        loopStack_.pop_back();
    }
    
    std::string CodeGen::loopLabel() const { return "loop_" + std::to_string(loopStack_.back()); }
    std::string CodeGen::endLoopLabel() const { return "endloop_" + std::to_string(loopStack_.back()); }
    std::string CodeGen::loopVariable() const { return "$counter_loop_" + std::to_string(loopStack_.back()); }
    
    ILBuilder::InsertLocation CodeGen::patchPoint() {
        return builder_.currentLocation();
    }
    
    void CodeGen::patchConversion(Opcode code, ILBuilder::InsertLocation at) {
        builder_.addInstruction(code, at);
        builder_.finishInstruction();
    }
    
    void CodeGen::patchCall(Opcode code, const std::string& symbol, ILBuilder::InsertLocation at) {
        auto& inst = builder_.addInstruction(code, at);
        inst.setOperand8(builder_.constant(symbol));
        builder_.finishInstruction();
    }
    
    void CodeGen::dropCode(ILBuilder::InsertLocation at) {
        builder_.removeInstruction(at);
    }
    
    void CodeGen::declareLocal(const tinyscript::Token &symbol) {
        auto name = manager_.tokenAsString(symbol);
        builder_.local(name);
    }
    
    void CodeGen::emitLabel(const std::string &label) {
        builder_.addSymbol(label);
    }
    
    void CodeGen::emitLocal(tinyscript::Opcode code, const Token& symbol) {
        auto name = manager_.tokenAsString(symbol);
        emitLocal(code, name);
    }
    
    void CodeGen::emitLocal(tinyscript::Opcode code, const std::string &symbol) {
        auto& inst = builder_.addInstruction(code);
        inst.setOperand8(builder_.local(symbol));
        builder_.finishInstruction();
    }
    
    void CodeGen::emitConstantI(tinyscript::Opcode code, const Token& symbol) {
        int64_t num = manager_.tokenAsInt(symbol);
        emitConstantI(code, num);
    }
    
    void CodeGen::emitConstantI(tinyscript::Opcode code, std::int64_t symbol) {
        auto& inst = builder_.addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.finishInstruction();
    }
    
    void CodeGen::emitConstantF(tinyscript::Opcode code, const Token& symbol) {
        float num = manager_.tokenAsFloat(symbol);
        emitConstantF(code, num);
    }
    
    void CodeGen::emitConstantF(tinyscript::Opcode code, float symbol) {
        auto& inst = builder_.addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.finishInstruction();
    }
    
    void CodeGen::emitConstantS(tinyscript::Opcode code, const Token& symbol) {
        emitConstantS(code, manager_.tokenAsString(symbol));
    }
    
    void CodeGen::emitConstantS(tinyscript::Opcode code, const std::string& symbol) {
        auto& inst = builder_.addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.finishInstruction();
    }
    
    void CodeGen::emitInstruction(tinyscript::Opcode code) {
        builder_.addInstruction(code);
        builder_.finishInstruction();
    }
    
    void CodeGen::emitJump(tinyscript::Opcode code, const std::string &label) {
        auto& inst = builder_.addInstruction(code);
        inst.setLabel(label);
        builder_.finishInstruction();
    }
    
    Program CodeGen::generate(bool dump) {
        Program prog;
        builder_.resolveReferences();
        builder_.write(prog);
        if(dump) builder_.dump(std::cerr);
        return prog;
    }
}
