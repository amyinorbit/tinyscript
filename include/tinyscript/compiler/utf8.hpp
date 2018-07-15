//===--------------------------------------------------------------------------------------------===
// orbit/utils/utf8.h
// This source is part of Orbit - Utils
//
// Created on 2017-02-06 by Amy Parent <amy@amyparent.com>
// Copyright (c) 2016-2018 Amy Parent <amy@amyparent.com>
// Available under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#pragma once
#include <cstdint>

namespace tinyscript {
    
    namespace utf8 {

        // Functions we need to deal with UTF8-encoded strings and files. The goal is
        // to have everything in Orbit be unicode-compatible (strings, but also names
        // in source files). The characters allowed as start and part of identifiers
        // are the same as the ones defined by clang for C/C++ and for Swift.

        typedef int32_t Codepoint;

        Codepoint getCodepoint(const char* data, uint64_t length);

        // writes [codepoint] to a [data]. If [point] takes more bytes than [length],
        // returns -1. Otherwise, returns the number of bytes written to [data].
        int8_t writeCodepoint(Codepoint point, char* data, uint64_t length);

        // Returns how many bytes are required to encode [point], or -1 if the codepoint
        // is not a valid one.
        int8_t codepointSize(Codepoint point);

        // Returns whether [point] is in a private Unicode range.
        bool isPrivate(Codepoint point);

        // Returns whether [point] is a Unicode combining character.
        bool isCombining(Codepoint point);

        // Returns whether [point] is in the Unicode Basic Multinlingual Plane.
        bool isBMP(Codepoint point);

        // Returns whether [point] is considered whitespace.
        bool isWhitespace(Codepoint point);

        // Returns whether [point] is a valid start of identifier character.
        // At the moment, those characters are exactly the same as in Swift.
        bool isIdentifierHead(Codepoint point);

        // Returns whether [point] is a valid identifier character.
        bool isIdentifier(Codepoint point);

        // Returns whether [point] is a valid operator character.
        bool isOperator(Codepoint point);

        // Returns whether [point] is a valid string literal character.
        bool isQuotedItem(Codepoint point);
    }
}
