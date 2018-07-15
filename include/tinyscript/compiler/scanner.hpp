//
//  scanner.hpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once
#include <tinyscript/compiler/token.hpp>
#include <tinyscript/compiler/utf8.hpp>

namespace tinyscript {
    class SourceManager;
    
    class Scanner {
    public:
        Scanner(const SourceManager& manager);
        void consumeToken();
        const Token& currentToken() const { return currentToken_; }
    private:
        
        utf8::Codepoint consume();
        utf8::Codepoint nextChar() const;
        utf8::Codepoint nextChar2() const;
        utf8::Codepoint currentChar() const;
        
        uint32_t remaining() const;
        
        
        void updateTokenStart();
        void makeToken(Token::Kind kind, int endOffset = 0);
        
        bool match(utf8::Codepoint c);
        void twoChars(utf8::Codepoint c, Token::Kind two, Token::Kind one);
        
        bool isDigit(utf8::Codepoint c) { return c >= '0' && c <= '9'; }
        
        void eatLineComment();
        void lexIdentifier();
        void lexNumber();
        void lexString();
        
        const SourceManager& manager_;
        
        bool isStartOfLine_;
        const char* currentPtr_;
        const char* tokenStart_;
        utf8::Codepoint current_;
        Token currentToken_;
    };
}
