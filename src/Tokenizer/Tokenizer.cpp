#include "Tokenizer.h"

#include "../Reader.h"
#include "../Logger/Logger.h"

namespace Radium {
    Tokenizer::Tokenizer() {}

    std::vector<Token> Tokenizer::tokenize(std::string_view source) {
        std::vector<Token> tokens;
        Reader<char> reader(std::vector(source.begin(), source.end()));

        std::vector<char> word;
        while (reader.peek().has_value()) {
            if (reader.peek().value() == '/' && reader.peek(1).has_value() && reader.peek(1).value() == '/') {
                reader.consumeUntil([](const char c) { return c == '\n'; });
                reader.consume();

                continue;
            }
            
            if (std::find(s_punctuators.begin(), s_punctuators.end(), reader.peek().value()) != s_punctuators.end()) {
                char c = reader.peek().value();
                std::optional<TokenType> type = Token::getTypeFromString(std::string_view(&c, 1));
                if (type.has_value()) tokens.emplace_back(type.value());
                else RA_ERROR("Found punctuator, {0}, but could not get TokenType", c);

                reader.consume();
                continue;
            }

            if (std::isspace(reader.peek().value())) {
                word.clear();
                reader.consume();

                continue;
            }

            // Begin word
            if (std::isalpha(reader.peek().value())) {
                word = reader.consumeUntil([this](const char c) { return isSpaceOrPunctuator(c); });

                std::optional<TokenType> type = Token::getTypeFromString(std::string_view(word.begin(), word.end()));
                // Keyword
                if (type.has_value()) {
                    tokens.emplace_back(type.value());
                }
                // Identifier
                else {
                    tokens.emplace_back(identifier, std::string(word.begin(), word.end()));
                }
            }
            else if (std::isdigit(reader.peek().value())) {
                // Assume int lit
                word = reader.consumeUntil([](const char c) { return !std::isdigit(c); });

                tokens.emplace_back(literal_int, std::string(word.begin(), word.end()));
            }
        }

        return tokens; 
    }
}
