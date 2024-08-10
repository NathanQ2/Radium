#include "Parser.h"

#include <iostream>

#include "../Logger/Logger.h"

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
                    RA_ERROR("Failed to parse exit staement!");
                    exit(EXIT_FAILURE);
                }

                continue;
            }
            if(peek().value().type == Tokenizer::TokenType::let)
            {
                if(auto letStatement = parseLet())
                {
                    statements.push_back(NodeStatement {.variant = letStatement.value()});
                }
                else
                {
                    RA_ERROR("Failed to parse variable declaration!");
                    exit(EXIT_FAILURE);
                }

                continue;
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

    bool Parser::ifType(int offset, Tokenizer::TokenType type)
    {
        return peek(offset).has_value() && peek(offset).value().type == type;
    }

    std::optional<NodeExpression> Parser::parseExpression()
    {
        auto intLit = tryParseExpressionIntLit();
        if (intLit.has_value() && !ifType(1, Tokenizer::TokenType::operator_add))
        {
            consume();
            return std::optional<NodeExpression>(intLit);
        }

        auto identifier = tryParseExpressionIdentifier();
        if (identifier.has_value() && !ifType(1, Tokenizer::TokenType::operator_add))
        {
            consume();
            return std::optional<NodeExpression>(identifier);
        }

        if (ifType(1, Tokenizer::TokenType::operator_add))
        {
            NodeExpression* lhs = nullptr;
            NodeExpression* rhs = nullptr;
            if(ifType(Tokenizer::TokenType::literal_int))
            {
                if(auto intLit = tryParseExpressionIntLit())
                {
                    consume();
                    lhs = new NodeExpression { .variant = intLit.value() };
                }
            }
            else if(ifType(Tokenizer::TokenType::identifier))
            {
                if (auto identifier = tryParseExpressionIdentifier())
                {
                    consume();
                    lhs = new NodeExpression { .variant = identifier.value() };
                }
            }
            consume();

            if(ifType(Tokenizer::TokenType::literal_int))
            {
                if(auto intLit = tryParseExpressionIntLit())
                {
                    consume();
                    rhs = new NodeExpression { .variant = intLit.value() };
                }
            }
            else if(ifType(Tokenizer::TokenType::identifier))
            {
                if (auto identifier = tryParseExpressionIdentifier())
                {
                    consume();
                    rhs = new NodeExpression { .variant = identifier.value() };
                }
            }

            if(lhs == nullptr || rhs == nullptr)
            {
                return std::nullopt;
            }

            return NodeExpression { .variant = NodeExpressionAdd { .lhs = lhs, .rhs = rhs }};
        }

        // if (peek().has_value() && peek().value().type == Tokenizer::TokenType::literal_int  &&
        //     (peek(1).has_value() && peek(1).value().type == Tokenizer::TokenType::operator_add) &&
        //     (peek(2).has_value() && peek(2).value().type == Tokenizer::TokenType::literal_int))
        // {
        //     const auto lhs = new NodeExpression { .variant = NodeExpressionIntLit {.value = peek().value().value.value()}};
        //     const auto rhs = new NodeExpression { .variant = NodeExpressionIntLit {.value = peek(2).value().value.value()}};
        //
        //     auto expr = std::optional<NodeExpression>(NodeExpressionAdd {.lhs = lhs, .rhs = rhs});
        //     consume();
        //     consume();
        //     consume();
        //
        //     return expr;
        // }
        //
        // if (peek().has_value() && peek().value().type == Tokenizer::TokenType::literal_int  &&
        //     (peek(1).has_value() && peek(1).value().type == Tokenizer::TokenType::operator_add) &&
        //     (peek(2).has_value() && peek(2).value().type == Tokenizer::TokenType::identifier))
        // {
        //     const auto lhs = new NodeExpression { .variant = NodeExpressionIntLit {.value = peek().value().value.value()}};
        //     const auto rhs = new NodeExpression { .variant = NodeExpressionIdentifer {.value = peek(2).value().value.value()}};
        //
        //     auto expr = std::optional<NodeExpression>(NodeExpressionAdd {.lhs = lhs, .rhs = rhs});
        //     consume();
        //     consume();
        //     consume();
        //
        //     return expr;
        // }

        return std::nullopt;
    }

    std::optional<NodeExpressionIntLit> Parser::tryParseExpressionIntLit()
    {
        if (ifType(Tokenizer::TokenType::literal_int))
        {
            auto expr = NodeExpressionIntLit {.value = peek().value().value.value()};

            return expr;
        }

        return std::nullopt;
    }

    std::optional<NodeExpressionIdentifier> Parser::tryParseExpressionIdentifier()
    {
        if(ifType(Tokenizer::TokenType::identifier) && !ifType(Tokenizer::TokenType::operator_add))
        {
            auto expr = NodeExpressionIdentifier {.value = peek().value().value.value()};

            return expr;
        }

        return std::nullopt;
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

    std::optional<NodeStatementLet> Parser::parseLet()
    {
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::let)
            return std::nullopt;

        consume();
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::identifier)
            return std::nullopt;

        std::string identifier = peek().value().value.value();

        consume();
        if(!peek().has_value() || peek().value().type != Tokenizer::TokenType::equal_single)
            return std::nullopt;

        consume();
        std::optional<NodeExpression> expression = parseExpression();
        if(!expression.has_value())
            return std::nullopt;

        if(!peek().has_value() || peek().value().type != Tokenizer::semicolon)
            return std::nullopt;

        consume();
        return NodeStatementLet {.identifier = identifier, .expression = expression.value()};
    }
}
