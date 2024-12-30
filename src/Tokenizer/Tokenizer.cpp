#include "Tokenizer.h"

#include <iostream>

namespace Radium
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
                tokens.emplace_back(parenthesis_open);
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == ')')
            {
                tokens.emplace_back(parenthesis_close);
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == ';')
            {
                tokens.emplace_back(semicolon);
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == '=' && !peekIs('=', 1))
            {
                tokens.emplace_back(equal_single);
                buf.clear();
                consume();

                continue;
            }

            if(peek().value() == '+' && peekIs(' ', 1))
            {
                tokens.emplace_back(operator_add);
                buf.clear();
                consume();

                continue;
            }

            if(std::isalpha(peek().value()))
            {
                while(std::isalnum(peek().value()))
                {
                    buf.push_back(consume().value());
                }

                if(buf == "exit")
                {
                    tokens.emplace_back(builtin_exit);
                    buf.clear();
                }
                else if(buf == "let")
                {
                    tokens.emplace_back(let);
                    buf.clear();

                }
                else // Assume this token is an identifier
                {
                    tokens.emplace_back(identifier, buf);
                    buf.clear();
                }
            }

            if(std::isdigit(peek().value()))
            {
                while(std::isdigit(peek().value()))
                {
                    buf.push_back(consume().value());
                }

                tokens.emplace_back(literal_int, buf);
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

    bool Tokenizer::peekIs(char a, const int offset)
    {
        return peek(offset).has_value() && peek(offset).value() == a;
    }
}
