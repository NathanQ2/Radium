#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace Radium {
    class Tokenizer {
    public:
        explicit Tokenizer(std::string_view source);

        void reset() { m_i = 0; };
        
        Token next();
        std::vector<Token> tokenize();
    private:
        std::string_view m_source;
        size_t m_i;
    };
}
