#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace Radium {
    class Tokenizer {
    public:
        explicit Tokenizer();

        std::vector<Token> tokenize(std::string_view source);
    private:
        static constexpr std::array<std::string_view, 6> s_keywords = { "let", "func", "ret", "if", "include", "mod" };
        static constexpr std::array<char, 9> s_punctuators = { '(', ')', '=', ';', '+', '-', '{', '}', ',' };
        
        static constexpr bool s_tokenizeIntLit = true;
        static constexpr bool s_tokenizeIdentifier = true;
        
        static bool isSpaceOrPunctuator(const char c) {
            return std::isspace(c) || std::find(s_punctuators.begin(), s_punctuators.end(), c) != s_punctuators.end();
        };
    };
}
