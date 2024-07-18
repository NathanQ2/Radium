#pragma once

namespace Radium::Tokenizer
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
        builtin_exit
    };
}