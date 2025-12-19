#pragma once

namespace Radium
{
    enum TokenType
    {
        parenthesis_open,
        parenthesis_close,
        let,
        identifier,
        equal_single,
        semicolon,
        literal_int,
        operator_add,
        operator_subtract,
        func,
        ret,
        mod,
        include,
        comma,
        _if,
        curly_open,
        curly_close
    };
}
