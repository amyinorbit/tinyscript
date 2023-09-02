//
//  scanner.cpp
//  tinyscript
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#include <iostream>
#include <tinyscript/compiler/scanner.hpp>
#include <tinyscript/compiler/token.hpp>
#include <tinyscript/compiler/sourcemanager.hpp>

namespace tinyscript {
    
    using namespace utf8;
    
    Scanner::Scanner(const SourceManager& sm) : manager_(sm) {
        currentPtr_ = manager_.begin();
        makeToken(Token::Kind::invalid);
    }
    
    std::uint32_t Scanner::remaining() const {
        return static_cast<std::uint32_t>(manager_.end() - currentPtr_);
    }
    
    Codepoint Scanner::consume() {
        if(!currentPtr_) return current_ = '\0';
        current_ = utf8::getCodepoint(currentPtr_, remaining());
        auto size = utf8::codepointSize(current_);
        if(size > 0 && current_ != '\0') currentPtr_ += size;
        if(current_ == '\n') isStartOfLine_ = true;
        return current_;
    }
    
    Codepoint Scanner::currentChar() const {
        return current_;
    }
    
    Codepoint Scanner::nextChar() const {
        return utf8::getCodepoint(currentPtr_, remaining());
    }
    
    Codepoint Scanner::nextChar2() const {
        auto r = remaining();
        auto next = utf8::getCodepoint(currentPtr_, r);
        auto nextSize = utf8::codepointSize(next);
        r -= nextSize;
        return utf8::getCodepoint(currentPtr_ + nextSize, r);
    }
    
    void Scanner::makeToken(Token::Kind kind, int endOffset) {
        currentToken_.kind = kind;
        currentToken_.location = static_cast<std::uint32_t>(tokenStart_ - manager_.begin());
        currentToken_.length = static_cast<std::uint32_t>(currentPtr_ - tokenStart_) + endOffset;
        currentToken_.isStartOfLine = isStartOfLine_;
        isStartOfLine_ = false;
    }
    
    bool Scanner::match(utf8::Codepoint c) {
        if(nextChar() != c) return false;
        consume();
        return true;
    }
    
    void Scanner::twoChars(Codepoint c, Token::Kind two, Token::Kind one) {
        makeToken(match(c) ? two : one);
    }
    
    void Scanner::eatLineComment() {
        while(nextChar() != '\n' && nextChar() != '\0') consume();
    }
    
    void Scanner::lexIdentifier() {
        while(utf8::isIdentifier(nextChar())) consume();
        makeToken(Token::identifierKind(tokenStart_, static_cast<std::uint32_t>(currentPtr_ - tokenStart_)));
    }
    
    void Scanner::lexNumber() {
        auto kind = Token::Kind::lit_integer;
        while(isDigit(nextChar())) consume();
        
        if(nextChar() == '.' && isDigit(nextChar2())) {
            kind = Token::Kind::lit_floating;
            consume();
            while(isDigit(nextChar())) consume();
        }
        makeToken(kind);
    }
    
    void Scanner::lexString() {
        match('"');
        updateTokenStart();
        while(nextChar() != '"') {
            consume();
        }
        consume();
        makeToken(Token::Kind::lit_string, -1);
    }
    
    void Scanner::updateTokenStart() {
        tokenStart_ = currentPtr_;
    }
    
    void Scanner::consumeToken() {
        if(currentToken_.kind == Token::Kind::eof) return;
        
        while(nextChar() != '\0' && currentPtr_ < manager_.end()) {
            updateTokenStart();
            Codepoint c = consume();
            
            switch (c) {
                case 0x0020:
                case 0x000d:
                case 0x0009:
                case 0x000b:
                case 0x000c:
                case 0x000a:
                    break;
                    
                // MARK: - single character tokens
                case '.': makeToken(Token::Kind::op_dot); return;
                case ':': makeToken(Token::Kind::colon); return;
                case ';': makeToken(Token::Kind::semicolon); return;
                case ',': makeToken(Token::Kind::comma); return;
                    
                case '{': makeToken(Token::Kind::brace_l); return;
                case '}': makeToken(Token::Kind::brace_r); return;
                case '[': makeToken(Token::Kind::bracket_l); return;
                case ']': makeToken(Token::Kind::bracket_r); return;
                case '(': makeToken(Token::Kind::paren_l); return;
                case ')': makeToken(Token::Kind::paren_r); return;
                    
                case '&': makeToken(Token::Kind::op_amp); return;
                case '+': makeToken(Token::Kind::op_plus); return;
                case '-': twoChars('>', Token::Kind::arrow, Token::Kind::op_minus); return;
                
                case '*': makeToken(Token::Kind::op_star); return;
                case '/': makeToken(Token::Kind::op_slash); return;
                case '"': lexString(); return;
                case '#': eatLineComment(); break;
                    
                case '>': twoChars('=', Token::Kind::op_gteq, Token::Kind::op_gt); return;
                case '<': twoChars('=', Token::Kind::op_lteq, Token::Kind::op_lt); return;
                case '=': twoChars('=', Token::Kind::op_eqeq, Token::Kind::op_eq); return;
                    
                    
                default:
                    if(utf8::isIdentifierHead(c)) {
                        lexIdentifier();
                    } else if(isDigit(c)) {
                        lexNumber();
                    }
                    else {
                        // TODO: ERROR
                        std::cerr << "INVALID TOKEN " << (currentPtr_ - manager_.begin()) << std::endl;
                        makeToken(Token::Kind::eof);
                    }
                    return;
            }
        }
        makeToken(Token::Kind::eof);
    }
}
