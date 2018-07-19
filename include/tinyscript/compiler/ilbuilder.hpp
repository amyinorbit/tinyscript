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
    
    class ILInstruction;
    class ILFunction;
    class ILBuilder;
    
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
        bool            complete_;
        bool            resolved_;
    };
    
    class ILFunction {
    public:
        
        void addSymbol(const std::string& label);
        ILInstruction& addInstruction(Opcode code);
        ILInstruction& addInstruction(Opcode code, std::uint64_t at);
        void finishInstruction();
        void removeInstruction(std::uint64_t at);
        std::uint64_t currentLocation() const;
        
        std::uint8_t local(const std::string& symbol);
        std::int64_t getAddress(const std::string& label);
        
        void resolveReferences();
        Program::Function build() const;
        void dump(std::ostream &out) const;
        
    private:
        
        ILInstruction*                          current_ = nullptr;
        std::vector<std::string>                locals_;
        std::map<std::string, std::uint64_t>    symbols_;
        std::vector<ILInstruction>              il_;
        std::uint64_t                           pc_ = 0;
    };
    
    class ILBuilder {
    public:
        ILFunction& openFunction(const std::string& signature);
        ILFunction& currentFunction() { return current_ ? *current_ : script_; }
        void closeFunction();
        void closeScript();
        
        std::uint8_t constant(std::int64_t num);
        std::uint8_t constant(float num);
        std::uint8_t constant(const std::string& str);
        
        void dump(std::ostream& out) const;
        void write(Program& program) const;
        
    private:
        ILFunction*                                 current_ = nullptr;
        ILFunction                                  script_;
        std::unordered_map<std::string, ILFunction> functions_;
        std::vector<Value>                          constants_;
    };
}
