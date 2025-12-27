#pragma once

namespace Radium
{
    enum class TokenType
    {
        eof,
        parenthesis_open,
        parenthesis_close,
        identifier,
        equal_single,
        semicolon,
        literal_int,
        plus,
        minus,
        comma,
        curly_open,
        curly_close,
        let,
        func,
        ret,
        mod,
        include,
        _if
    };
}
