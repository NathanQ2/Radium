#include "Parser.h"

#include <iostream>

namespace Radium::Parser
{
    Parser::Parser(std::vector<Tokenizer::Token> tokens)
        : m_tokens(std::move(tokens)),
        m_index(0)
    {
    }

    NodeRoot Parser::parse()
    {
        std::vector<NodeStatement> statements;
        while(peek().has_value())
        {
            if(peek().value().type == Tokenizer::TokenType::builtin_exit)
            {
                if (auto exitStatement = parseExit())
                {
                    statements.push_back(NodeStatement {.variant = exitStatement.value()});
                }
                else
                {
                    std::cerr << "Failed to parse exit statement!" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }

        return NodeRoot{.statements = std::move(statements)};
    }

    std::optional<Tokenizer::Token> Parser::peek(int offset)
    {
        if(m_index + offset >= m_tokens.size())
            return std::nullopt;

        return m_tokens.at(m_index + offset);
    }

    std::optional<Tokenizer::Token> Parser::consume()
    {
        return m_tokens.at(m_index++);
    }

    std::optional<NodeExpression> Parser::parseExpression()
    {
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::literal_int)
            return std::nullopt;

        auto expr = std::optional<NodeExpression>(NodeExpressionIntLit {.value = peek().value().value.value()});
        consume();
        return expr;
    }

    std::optional<NodeStatementExit> Parser::parseExit()
    {
        // Expects current token to be of type bultin_exit
        if (!peek().has_value() || peek().value().type != Tokenizer::TokenType::builtin_exit)
            return std::nullopt;

        // Expect next token to be of type parenthesis_open
        consume();
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::parenthesis_open)
            return std::nullopt;

        // Expect next token to be of type expression
        consume();
        std::optional<NodeExpression> expression = parseExpression();

        if(!expression.has_value())
            return std::nullopt;

        if(!peek().has_value() || peek().value().type !=Tokenizer::TokenType::parenthesis_close)
            return std::nullopt;

        consume();
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::semicolon)
            return std::nullopt;

        consume();
        return NodeStatementExit {.expression = expression.value()};
    }
}
