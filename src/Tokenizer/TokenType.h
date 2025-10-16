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
        func,
        ret,
        comma,
        curly_open,
        curly_close
    };
}
