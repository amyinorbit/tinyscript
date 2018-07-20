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
#include <tinyscript/runtime/vm.hpp>

namespace tinyscript {
    
    CodeGen::CodeGen(const SourceManager& manager) : manager_(manager) {}
    
    void CodeGen::openIf() {
        ifStack_.push_back(ifID_++);
    }
    
    void CodeGen::closeIf() {
        builder_.currentFunction().addSymbol(endifLabel());
        ifStack_.pop_back();
    }
    
    std::string CodeGen::ifLabel() const { return "if_" + std::to_string(ifStack_.back()); }
    std::string CodeGen::elseLabel() const { return "else_" + std::to_string(ifStack_.back());}
    std::string CodeGen::endifLabel() const { return "endif_" + std::to_string(ifStack_.back());}
    
    void CodeGen::openLoop() {
        loopStack_.push_back(loopID_++);
    }
    
    void CodeGen::closeLoop() {
        builder_.currentFunction().addSymbol(endLoopLabel());
        loopStack_.pop_back();
    }
    
    std::string CodeGen::loopLabel() const { return "loop_" + std::to_string(loopStack_.back()); }
    std::string CodeGen::endLoopLabel() const { return "endloop_" + std::to_string(loopStack_.back()); }
    std::string CodeGen::loopVariable() const { return "$counter_loop_" + std::to_string(loopStack_.back()); }
    
    std::uint64_t CodeGen::patchPoint() {
        return builder_.currentFunction().currentLocation();
    }
    
    void CodeGen::patchConversion(Opcode code, std::uint64_t at) {
        builder_.currentFunction().addInstruction(code, at);
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::patchCall(Opcode code, const std::string& symbol, std::uint64_t at) {
        auto& inst = builder_.currentFunction().addInstruction(code, at);
        inst.setOperand8(builder_.constant(symbol));
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::dropCode(std::uint64_t at) {
        builder_.currentFunction().removeInstruction(at);
    }
    
    void CodeGen::openFunction(const Token& symbol, std::uint8_t arity) {
        auto signature = VM::mangleFunc(manager_.tokenAsString(symbol), arity);
        builder_.openFunction(signature, arity);
    }
    
    void CodeGen::closeFunction() {
        builder_.currentFunction().addInstruction(Opcode::ret);
        builder_.currentFunction().finishInstruction();
        builder_.closeFunction();
    }
    
    void CodeGen::declareLocal(const tinyscript::Token &symbol) {
        auto name = manager_.tokenAsString(symbol);
        builder_.currentFunction().local(name);
    }
    
    void CodeGen::emitLabel(const std::string &label) {
        builder_.currentFunction().addSymbol(label);
    }
    
    void CodeGen::emitLocal(tinyscript::Opcode code, const Token& symbol) {
        auto name = manager_.tokenAsString(symbol);
        emitLocal(code, name);
    }
    
    void CodeGen::emitLocal(tinyscript::Opcode code, const std::string &symbol) {
        auto& inst = builder_.currentFunction().addInstruction(code);
        inst.setOperand8(builder_.currentFunction().local(symbol));
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::emitConstantI(tinyscript::Opcode code, const Token& symbol) {
        int64_t num = manager_.tokenAsInt(symbol);
        emitConstantI(code, num);
    }
    
    void CodeGen::emitConstantI(tinyscript::Opcode code, std::int64_t symbol) {
        auto& inst = builder_.currentFunction().addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::emitConstantF(tinyscript::Opcode code, const Token& symbol) {
        float num = manager_.tokenAsFloat(symbol);
        emitConstantF(code, num);
    }
    
    void CodeGen::emitConstantF(tinyscript::Opcode code, float symbol) {
        auto& inst = builder_.currentFunction().addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::emitConstantS(tinyscript::Opcode code, const Token& symbol) {
        emitConstantS(code, manager_.tokenAsString(symbol));
    }
    
    void CodeGen::emitConstantS(tinyscript::Opcode code, const std::string& symbol) {
        auto& inst = builder_.currentFunction().addInstruction(code);
        inst.setOperand8(builder_.constant(symbol));
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::emitInstruction(tinyscript::Opcode code) {
        builder_.currentFunction().addInstruction(code);
        builder_.currentFunction().finishInstruction();
    }
    
    void CodeGen::emitJump(tinyscript::Opcode code, const std::string &label) {
        auto& inst = builder_.currentFunction().addInstruction(code);
        inst.setLabel(label);
        builder_.currentFunction().finishInstruction();
    }
    
    Program CodeGen::generate(bool dump) {
        Program prog;
        builder_.closeScript();
        builder_.write(prog);
        if(dump) builder_.dump(std::cerr);
        return prog;
    }
}
