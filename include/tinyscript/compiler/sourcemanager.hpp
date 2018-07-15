//
//  SourceManager.hpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <tinyscript/compiler/token.hpp>

namespace tinyscript {
    class SourceManager {
    public:
        
        SourceManager(std::istream& input);
        SourceManager(const std::string& input);
        ~SourceManager();
        
        void printLine(std::ostream& out, std::uint32_t location) const;
        void printLineAndToken(std::ostream& out, std::uint32_t location, std::uint32_t length) const;
        
        std::string tokenAsString(const Token& token) const;
        std::int64_t tokenAsInt(const Token& token) const;
        float tokenAsFloat(const Token& token) const;
        
        const char* end() const { return source_ + length_; }
        const char* begin() const { return source_; }
        
    private:
        char*           source_;
        std::uint32_t   length_;
    };
}
