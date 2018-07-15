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
    
    void ILInstruction::write(tinyscript::Program &program) const {
        if(!isResolved() || !isComplete()) return;
        program.bytecode.push_back(static_cast<std::uint8_t>(code_));
        
        switch (operandSize(code_)) {
            case 1:
                program.bytecode.push_back(operand_ & 0x00ff);
                break;
                
            case 2:
                program.bytecode.push_back((operand_ >> 8) & 0x00ff);
                program.bytecode.push_back(operand_ & 0x00ff);
                break;
                
            default:
                break;
        }
    }
    
    // MARK: - ILBuilder
    
    void ILBuilder::addSymbol(const std::string& label) {
        if(symbols_.find(label) != symbols_.end()) return;
        symbols_[label] = pc_;
    }
    
    ILInstruction& ILBuilder::addInstruction(Opcode code) {
        il_.push_back(ILInstruction(code, 0));
        current_ = &il_.back();
        return *current_;
    }
    
    ILInstruction& ILBuilder::addInstruction(tinyscript::Opcode code, InsertLocation at) {
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
    
    void ILBuilder::finishInstruction() {
        assert(current_ && "No open instruction");
        if(!current_->isComplete()) return; // TODO: some signalling?
        pc_ += 1;
    }
    
    void ILBuilder::removeInstruction(InsertLocation at) {
        assert(at < il_.size() && "Patched isntructions must be insterted at an existing point in the IL bitcode");
        //auto& instr = il_[at];
        
        // We need to update all symbols that come after too
        //auto address = il_[at].address();
        for(auto& pair: symbols_) {
            if(pair.second <= at) continue;
            pair.second -= 1;
        }
        il_.erase(il_.begin() + at);
        pc_ -= 1;
    }
    
    ILBuilder::InsertLocation ILBuilder::currentLocation() const {
        return il_.size();
    }
    
    std::uint8_t ILBuilder::constant(std::int64_t num) {
        auto val = Value::Integer(num);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(locals_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    std::uint8_t ILBuilder::constant(float num) {
        auto val = Value::Float(num);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(locals_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    std::uint8_t ILBuilder::constant(const std::string& str) {
        Value val(str);
        for(uint8_t i = 0; i < constants_.size(); ++i) {
            if(constants_[i] == val) return i;
        }
        assert(locals_.size() < 256 && "constants overflow");
        constants_.push_back(val);
        return constants_.size()-1;
    }
    
    std::uint8_t ILBuilder::local(const std::string& symbol) {
        for(uint8_t i = 0; i < locals_.size(); ++i) {
            if(locals_[i] == symbol) return i;
        }
        assert(locals_.size() < 256 && "locals overflow");
        locals_.push_back(symbol);
        return locals_.size()-1;
    }
    
    std::int64_t ILBuilder::getAddress(const std::string &label) {
        auto it = symbols_.find(label);
        if(it == symbols_.end()) return -1;
        return il_[it->second].address;
    }
    
    void ILBuilder::resolveReferences() {
        //auto rem = std::remove_if(il_.begin(), il_.end(), [](const ILInstruction& a) { return a.code() == Opcode::nop; });
        //il_.erase(rem, il_.end());
        
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
        out << "\nBytecode:" << std::endl;
        for(const auto& inst: il_) {
            std::cerr << "  " << inst.code();
            switch (inst.size()) {
                case 2:
                    out << " \t#" << (inst.operand() & 0x00ff);
                    if(inst.code() == Opcode::load_c) out << "\t(" << constants_[inst.operand()].repr() << ")";
                    break;
                case 3:
                    out << " \t->" << (inst.operand() & 0xffff);
                    break;
                default: break;
            }
            out << std::endl;
        }
        out << "--done" << std::endl;
    }
    
    void ILBuilder::write(tinyscript::Program &program) const {
        program.bytecode.clear();
        program.constants.clear();
        for(const auto& c: constants_) {
            program.constants.push_back(c);
        }
        program.variableCount = locals_.size();
        for(const auto& inst: il_) {
            inst.write(program);
        }
    }
}
