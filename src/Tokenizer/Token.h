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
    };
}
