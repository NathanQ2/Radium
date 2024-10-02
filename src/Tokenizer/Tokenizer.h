#pragma once

#include <string>
#include <vector>

#include "Token.h"

namespace Radium
{
    class Tokenizer
    {
    public:
        explicit Tokenizer(std::string  source);

        std::vector<Token> tokenize();

    private:
        std::optional<char> peek(int offset = 0);
        std::optional<char> consume();

        bool peekIs(char a, int offset = 1);

        std::string m_source;
        size_t m_index;
    };
}