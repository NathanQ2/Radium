#pragma once

#include <string>
#include <vector>

#include "Token.h"
#include "../Reader.h"

namespace Radium
{
    class Tokenizer
    {
    public:
        explicit Tokenizer(std::string  source);

        std::vector<Token> tokenize();

    private:
        Reader<char> m_reader;

        std::string m_source;
    };
}
