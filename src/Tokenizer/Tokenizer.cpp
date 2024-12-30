#include "Tokenizer.h"

#include <iostream>
#include <utility>

namespace Radium
{
    Tokenizer::Tokenizer(std::string source)
        : m_reader(std::vector(source.begin(), source.end())), m_source(std::move(source))
    {
    }

    std::vector<Token> Tokenizer::tokenize()
    {
        std::vector<Token> tokens = std::vector<Token>();

        std::string buf;
        while(m_reader.peek().has_value())
        {
            if(std::isspace(m_reader.peek().value()))
            {
                m_reader.consume();
                buf.clear();

                continue;
            }

            if (m_reader.peek().value() == '(')
            {
                tokens.emplace_back(parenthesis_open);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if(m_reader.peek().value() == ')')
            {
                tokens.emplace_back(parenthesis_close);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if (m_reader.peek().value() == '{')
            {
                tokens.emplace_back(curly_open);
                buf.clear();
                m_reader.consume();

                continue;
            }
            
            if (m_reader.peek().value() == '}')
            {
                tokens.emplace_back(curly_close);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if(m_reader.peek().value() == ';')
            {
                tokens.emplace_back(semicolon);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if(m_reader.peek().value() == '=' && !m_reader.peekAnd([](const char& c) { return c == '='; }, 1))
            {
                tokens.emplace_back(equal_single);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if(m_reader.peek().value() == '+')
            {
                tokens.emplace_back(operator_add);
                buf.clear();
                m_reader.consume();

                continue;
            }

            if (m_reader.peekAnd([](const char& c) { return c == '/'; }) && m_reader.peekAnd([](const char& c) { return c == '/'; }, 1))
            {
                while (m_reader.peek().has_value() && m_reader.peek().value() != '\n')
                {
                    m_reader.consume();
                }
                m_reader.consume();
                buf.clear();

                continue;
            }

            if(std::isalpha(m_reader.peek().value()))
            {
                while(std::isalnum(m_reader.peek().value()))
                {
                    buf.push_back(m_reader.consume());
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
                else if (buf == "func")
                {
                    tokens.emplace_back(func);
                }
                else // Assume this token is an identifier
                {
                    tokens.emplace_back(identifier, buf);
                    buf.clear();
                }
            }

            if(std::isdigit(m_reader.peek().value()))
            {
                while(std::isdigit(m_reader.peek().value()))
                {
                    buf.push_back(m_reader.consume());
                }

                tokens.emplace_back(literal_int, buf);
                buf.clear();

                continue;
            }
        }

        return tokens;
    }
}
