//
//  Type.hpp
//  tinyscript
//
//  Created by Amy Parent on 04/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//
#pragma once

namespace tinyscript {
    enum Type {
        Invalid = 0,
        Void    = 1,
        Bool    = 1 << 1,
        Integer = 1 << 2,
        Number  = 1 << 3,
        String  = 1 << 4,
    };
    
    enum class TypeConversion {
        Invalid,
        FloatToInt,
        IntToFloat,
        IntToString,
        FloatToString,
    };
    
    static inline bool isNumeric(Type type) {
        return type == Type::Integer || type == Type::Number;
    }
    
    static inline bool isConcrete(Type type) {
        return type != Type::Invalid && type != Type::Void;
    }
}
