#pragma once

#include <string>
#include <optional>
#include <utility>

#include "TokenType.h"

namespace Radium::Tokenizer
{
    struct Token {
        TokenType type;
        std::optional<std::string> value;

        static std::optional<int> binPrecedence(const TokenType type)
        {
            switch (type)
            {
            case bin_operator_add:
                return 0;
            case bin_operator_mult:
                return 1;
            default:
                return std::nullopt;
            }
        }

        static bool isBinOperator(const TokenType type)
        {
            return binPrecedence(type).has_value();
        }
    };
}
