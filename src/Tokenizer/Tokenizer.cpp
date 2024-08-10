#include "Tokenizer.h"

#include <iostream>
#include <utility>

namespace Radium::Tokenizer
{
    Tokenizer::Tokenizer(std::string  source)
        : m_source(std::move(source)), m_index(0)
    {
    }

    std::vector<Token> Tokenizer::tokenize()
    {
        std::vector<Token> tokens = std::vector<Token>();

        std::string buf;
        while(peek().has_value())
        {
            if(std::isspace(peek().value()))
            {
                consume();
                buf.clear();
                continue;
            }

            if (peek().value() == '(')
            {
                tokens.push_back(Token {.type = parenthesis_open, .value = std::nullopt});
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == ')')
            {
                tokens.push_back(Token {.type = parenthesis_close, .value = std::nullopt});
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == ';')
            {
                tokens.push_back(Token {.type = semicolon, .value = std::nullopt});
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == '=' && (!peek(1).has_value() || peek(1).value() != '='))
            {
                tokens.push_back(Token {.type = equal_single});
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == '+' && (!peek(1).has_value() || peek(1).value() == ' '))
            {
                tokens.push_back(Token { .type = operator_add });
                buf.clear();
                consume();

                continue;
            }

            if(std::isalpha(peek().value()))
            {
                while(std::isalpha(peek().value()))
                {
                    buf.push_back(consume().value());
                }

                if(buf == "exit")
                {
                    tokens.push_back(Token {.type = builtin_exit});
                    buf.clear();

                }
                else if(buf == "let")
                {
                    tokens.push_back(Token {.type = let});
                    buf.clear();

                }
                else // Assume this token is an identifier
                {
                    tokens.push_back(Token {.type = identifier, .value = buf});
                    buf.clear();
                }
            }

            if(std::isdigit(peek().value()))
            {
                while(std::isdigit(peek().value()))
                {
                    buf.push_back(consume().value());
                }

                tokens.push_back(Token {.type = literal_int, .value = buf});
                buf.clear();

                continue;
            }
        }

        return tokens;
    }

    std::optional<char> Tokenizer::peek(const int offset)
    {
        if (m_index + offset >= m_source.length())
            return  std::nullopt;

        return m_source.at(m_index + offset);
    }

    std::optional<char> Tokenizer::consume()
    {
        return m_source.at(m_index++);
    }
}
