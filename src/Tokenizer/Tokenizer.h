#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace Radium {
    struct TokenizerConfiguration {
        bool tokenizeIntLit = false;
        bool tokenizeIdentifier = false;

        std::vector<char> punctuators;
        std::vector<std::string_view> keywords;
    };
    
    class Tokenizer {
    public:
        explicit Tokenizer(const TokenizerConfiguration& config);

        std::vector<Token> tokenize(std::string_view source);
    private:
        bool isSpaceOrPunctuator(const char c) {
            return std::isspace(c) || std::find(m_punctuators.begin(), m_punctuators.end(), c) != m_punctuators.end();
        };
        
        const std::vector<std::string_view>& m_keywords;
        const std::vector<char>& m_punctuators;

        bool m_tokenizeIntLit = false;
        bool m_tokenizeIdentifier = false;
    };
}
