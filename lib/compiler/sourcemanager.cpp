//
//  SourceManager.cpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

#include <tinyscript/compiler/sourcemanager.hpp>

namespace tinyscript {
    
    SourceManager::SourceManager(std::istream& input) {
        std::string source(std::istreambuf_iterator<char>(input), {});
        length_ = static_cast<std::uint32_t>(source.length());
        source_ = new char[length_];
        std::copy(source.begin(), source.end(), source_);
    }
    
    SourceManager::SourceManager(const std::string& source) {
        length_ = static_cast<std::uint32_t>(source.length());
        source_ = new char[length_];
        std::copy(source.begin(), source.end(), source_);
    }
    
    SourceManager::~SourceManager() {
        delete[] source_;
    }
    
    std::string SourceManager::tokenAsString(const tinyscript::Token &token) const {
        return std::string(source_ + token.location, token.length);
    }
    
    std::int64_t SourceManager::tokenAsInt(const Token& token) const {
        return std::stoll(tokenAsString(token));
    }
    
    float SourceManager::tokenAsFloat(const tinyscript::Token &token) const {
        return std::stof(tokenAsString(token));
    }
    
    void SourceManager::printLine(std::ostream& out, std::uint32_t location) const {
        const char* loc = source_ + location;
        const char* start = loc;
        while(*(start-1) != '\n'&& start != source_) {
            start -= 1;
        }
        
        const char* stop = loc;
        while(*stop != '\n' && stop != end()) {
            stop += 1;
        }
        
        std::string line(start, stop-start);
        std::cout << line << std::endl;
    }
    
    void SourceManager::printLineAndToken(std::ostream& out, std::uint32_t location, std::uint32_t length) const {
        const char* loc = source_ + location;
        const char* start = loc;
        while(*(start-1) != '\n'&& start != source_) {
            start -= 1;
        }
        
        const char* stop = loc;
        while(*stop != '\n' && stop != end()) {
            stop += 1;
        }
        
        std::string line(start, stop-start);
        std::cout << line << std::endl;
        
        auto offset = loc - start;
        for(int i = 0; i < offset; ++i) std::cout << " ";
        std::cout << "^";
        for(int i = 1; i < length; ++i) std::cout << "~";
        std::cout << std::endl;
        
    }
}
