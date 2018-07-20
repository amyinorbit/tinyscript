//
//  ilbuilder.cpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <cassert>
#include <iostream>
#include <algorithm>
#include <tinyscript/compiler/ilbuilder.hpp>

namespace tinyscript {
    ILInstruction::ILInstruction(Opcode code, std::uint64_t address) {
        code_ = code;
        //address_ = address;
        complete_ = operandSize(code) == 0;
        resolved_ = complete_;
    }
    
    void ILInstruction::setLabel(const std::string &label) {
        assert(operandSize(code_) == 2 && "Invalid operand size");
        label_ = label;
        complete_ = true;
        resolved_ = false;
    }
    
    void ILInstruction::replaceLabel(std::uint16_t op) {
        assert(operandSize(code_) == 2 && "Invalid operand size");
        assert(!resolved_ && "instruction is already resolved");
        assert(complete_ && "instruction is not complete");
        operand_ = op;
        resolved_ = true;
    }
    
    void ILInstruction::setOperand8(std::uint8_t op) {
        assert(operandSize(code_) == 1 && "Invalid operand size");
        assert(!complete_ && "instruction is already complete");
        operand_ = op;
        complete_ = resolved_ = true;
    }
    
    void ILInstruction::setOperand16(std::uint16_t op) {
        assert(operandSize(code_) == 2 && "Invalid operand size");
        assert(!complete_ && "instruction is already complete");
        operand_ = op;
        complete_ = resolved_ = true;
    }
    
    void ILInstruction::write(Program::Function& function) const {
        if(!isResolved() || !isComplete()) return;
        function.bytecode.push_back(static_cast<std::uint8_t>(code_));
        
        switch (operandSize(code_)) {
            case 1:
                function.bytecode.push_back(operand_ & 0x00ff);
                break;
                
            case 2:
                function.bytecode.push_back((operand_ >> 8) & 0x00ff);
                function.bytecode.push_back(operand_ & 0x00ff);
                break;
                
            default:
                break;
        }
    }
    
    // MARK: - ILFunction
    
    void ILFunction::addSymbol(const std::string& label) {
        if(symbols_.find(label) != symbols_.end()) return;
        symbols_[label] = pc_;
    }
    
    ILInstruction& ILFunction::addInstruction(Opcode code) {
        il_.push_back(ILInstruction(code, 0));
        current_ = &il_.back();
        return *current_;
    }
    
    ILInstruction& ILFunction::addInstruction(tinyscript::Opcode code, std::uint64_t at) {
        if(at == il_.size()) return addInstruction(code);
        assert(at < il_.size() && "Patched isntructions must be insterted at an existing point in the IL bitcode");
        
        ILInstruction instr(code, 0);
        il_.insert(il_.begin() + at, instr);
        current_ = &il_[at];
        
        for(auto& pair: symbols_) {
            if(pair.second <= at) continue;
            pair.second += 1;
        }
        return *current_;
    }
    
    void ILFunction::finishInstruction() {
        assert(current_ && "No open instruction");
        if(!current_->isComplete()) return; // TODO: some signalling?
        pc_ += 1;
    }
    
    void ILFunction::removeInstruction(std::uint64_t at) {
        assert(at < il_.size() && "Patched instructions must be inserted at an existing point in the IL bitcode");
        
        // We need to update all symbols that come after too
        //auto address = il_[at].address();
        for(auto& pair: symbols_) {
            if(pair.second <= at) continue;
            pair.second -= 1;
        }
        il_.erase(il_.begin() + at);
        pc_ -= 1;
    }
    
    std::uint64_t ILFunction::currentLocation() const {
        return il_.size();
    }
    
    std::uint8_t ILFunction::local(const std::string& symbol) {
        for(uint8_t i = 0; i < locals_.size(); ++i) {
            if(locals_[i] == symbol) return i;
        }
        assert(locals_.size() < 256 && "locals overflow");
        locals_.push_back(symbol);
        return locals_.size()-1;
    }
    
    std::int64_t ILFunction::getAddress(const std::string &label) {
        auto it = symbols_.find(label);
        if(it == symbols_.end()) return -1;
        return il_[it->second].address;
    }
    
    void ILFunction::resolveReferences() {
        auto rem = std::remove_if(il_.begin(), il_.end(), [](const ILInstruction& a) { return a.code() == Opcode::nop; });
        il_.erase(rem, il_.end());
        
        std::uint64_t address = 0;
        for(auto& inst: il_) {
            inst.address = address;
            address += inst.size();
        }
        
        
        for(auto& inst: il_) {
            if(inst.isResolved()) continue;
            std::int64_t target = getAddress(inst.label());
            assert(target >= 0 && "Wrong label found");
            std::int64_t pc = inst.address + inst.size();
            std::uint16_t jump = std::abs(pc-target);
            inst.replaceLabel(jump);
        }
    }
    
    Program::Function ILFunction::build() const {
        Program::Function function;
        function.bytecode.clear();
        function.variableCount = locals_.size();
        function.arity = arity_;
        for(const auto& inst: il_) {
            inst.write(function);
        }
        return function;
    }
    
    void ILFunction::dump(std::ostream &out) const {
        for(const auto& inst: il_) {
            out << "\t" << inst.code();
            switch (inst.size()) {
                case 2: out << " \t#" << (inst.operand() & 0x00ff); break;
                case 3: out << " \t->" << (inst.operand() & 0xffff); break;
                default: break;
            }
            out << std::endl;
        }
        out << std::endl;
    }
    
    // MARK: - ILBuilder
    
    ILFunction& ILBuilder::openFunction(const std::string& signature, std::uint8_t arity) {
        assert(current_ == nullptr && "a function is already opened");
        assert(functions_.find(signature) == functions_.end() && "function already exists");
        functions_[signature] = ILFunction(arity);
        current_ = &functions_.at(signature);
        return *current_;
    }
    
    void ILBuilder::closeFunction() {
        assert(current_ != nullptr && "no open function");
        current_->resolveReferences();
        current_ = nullptr;
    }
    
    void ILBuilder::closeScript() {
        script_.resolveReferences();
    }
    
    std::uint8_t ILBuilder::constant(std::int64_t num) {
        auto val = Value::Integer(num);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(constants_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    std::uint8_t ILBuilder::constant(float num) {
        auto val = Value::Float(num);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(constants_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    std::uint8_t ILBuilder::constant(const std::string& str) {
        Value val(str);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(constants_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    void ILBuilder::dump(std::ostream &out) const {
        out << "Constants:" << std::endl;
        int i = 0;
        for(const auto& c: constants_) {
            out << "  " << i++ << ":\t";
            switch(c.kind) {
                case Value::Kind::String:   out << "String:  '" << c.asString() << "'"; break;
                case Value::Kind::Number:   out << "Float:   " << c.asNumber(); break;
                case Value::Kind::Int:      out << "Integer: " << c.asInt(); break;
                default: break;
            }
            out << std::endl;
        }
        
        out << "--bytecode (main script):" << std::endl;
        script_.dump(out);
        
        for(const auto& pair: functions_) {
            out << "--bytecode (" << pair.first << "):" << std::endl;
            pair.second.dump(out);
        }
        out << "--done" << std::endl;
    }
    
    void ILBuilder::write(tinyscript::Program &program) const {
        program.constants.clear();
        program.functions.clear();
        
        for(const auto& c: constants_) {
            program.constants.push_back(c);
        }
        
        program.script = script_.build();
        for(const auto& pair: functions_) {
            program.functions[pair.first] = pair.second.build();
        }
    }
}
