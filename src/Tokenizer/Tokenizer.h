#pragma once

#include <string>
#include <vector>

#include "Token.h"

namespace Radium::Tokenizer
{
    class Tokenizer
    {
    public:
        explicit Tokenizer(std::string  source);

        std::vector<Token> tokenize();

    private:
        std::optional<char> peek(int offset = 0);
        std::optional<char> consume();

        std::string m_source;
        size_t m_index;
    };
}