//
//  ilbuilder.hpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>

#include <tinyscript/opcodes.hpp>
#include <tinyscript/runtime/program.hpp>
#include <tinyscript/runtime/value.hpp>

namespace tinyscript {
    
    class ILInstruction {
    public:
        ILInstruction(Opcode code, std::uint64_t address);
        
        std::uint64_t size() const { return code_ == Opcode::nop ? 1 : 1 + operandSize(code_); }
        
        //void moveAddress(std::int64_t offset) { address_ += offset; }
        void nop() { code_ = Opcode::nop; complete_ = true; resolved_ = true; }
        
        void setLabel(const std::string& label);
        void replaceLabel(std::uint16_t op);
        void setOperand8(std::uint8_t op);
        void setOperand16(std::uint16_t op);
        
        bool isComplete() const { return complete_; }
        bool isResolved() const { return resolved_; }
        //std::uint64_t address() const { return address_; }
        
        const std::string& label() const { return label_; }
        Opcode code() const { return code_; }
        std::uint16_t operand() const { return operand_; }
        void write(Program::Function& function) const;
        
        std::uint64_t address;
    private:
        Opcode          code_;
        std::uint16_t   operand_;
        std::string     label_;
        //std::uint64_t   address_;
        bool            complete_;
        bool            resolved_;
    };
    
    class ILBuilder {
    public:
        
        using InsertLocation = std::uint64_t;
        
        void addSymbol(const std::string& label);
        ILInstruction& addInstruction(Opcode code);
        ILInstruction& addInstruction(Opcode code, InsertLocation at);
        void finishInstruction();
        void removeInstruction(InsertLocation at);
        InsertLocation currentLocation() const;
        
        void resolveReferences();
        
        std::uint8_t constant(std::int64_t num);
        std::uint8_t constant(float num);
        std::uint8_t constant(const std::string& str);
        std::uint8_t local(const std::string& symbol);
        std::int64_t getAddress(const std::string& label);
        
        void dump(std::ostream& out) const;
        void write(Program& program) const;
        
    private:
        ILInstruction*                          current_ = NULL;
        std::vector<Value>                      constants_;
        std::vector<std::string>                locals_;
        std::map<std::string, std::uint64_t>    symbols_;
        std::vector<ILInstruction>              il_;
        std::uint64_t                           pc_ = 0;
    };
}
