#include "Parser.h"

#include <iostream>

#include "../Logger/Logger.h"

namespace Radium
{
    Parser::Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)),
        m_index(0)
    {
    }

    NodeRoot Parser::parse()
    {
        std::vector<NodeStatement> statements;
        while(peek().has_value())
        {
            if(peek().value().type == TokenType::builtin_exit)
            {
                if (auto exitStatement = parseExit())
                {
                    statements.push_back(NodeStatement {.variant = exitStatement.value()});
                }
                else
                {
                    RA_ERROR("Failed to parse exit statement!");
                    exit(EXIT_FAILURE);
                }

                break;
            }
            if(peek().value().type == TokenType::let)
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

    std::optional<Token> Parser::peek(int offset)
    {
        if(m_index + offset >= m_tokens.size())
            return std::nullopt;

        return m_tokens.at(m_index + offset);
    }

    std::optional<Token> Parser::consume(int offset)
    {
        m_index += offset;
        return m_tokens.at(m_index);
    }

    bool Parser::ifType(int offset, TokenType type)
    {
        return peek(offset).has_value() && peek(offset).value().type == type;
    }

    std::optional<NodeExpression> Parser::parseExpression(int offset)
    {
        // just int lit
        auto intLit = tryParseExpressionIntLit(offset);
        if (intLit.has_value() && !ifType(1, TokenType::operator_add))
        {
            consume(offset + 1);
            return std::optional<NodeExpression>(intLit);
        }

        // just identifier
        auto identifier = tryParseExpressionIdentifier(offset);
        if (identifier.has_value() && !ifType(1, TokenType::operator_add))
        {
            consume(offset + 1);
            return std::optional<NodeExpression>(identifier);
        }

        // add operator to the right
        if (ifType(1, TokenType::operator_add))
        {
            NodeExpression* lhs = nullptr;
            NodeExpression* rhs = nullptr;

            if(ifType(TokenType::literal_int))
            {
                if(auto intLit = tryParseExpressionIntLit(offset))
                {
                    consume(offset + 1);
                    lhs = new NodeExpression { .variant = intLit.value() };
                }
            }
            else if(ifType(TokenType::identifier))
            {
                if (auto identifier = tryParseExpressionIdentifier())
                {
                    consume(offset + 1);
                    lhs = new NodeExpression { .variant = identifier.value() };
                }
            }
            consume(offset + 1);

            auto rhExpr = parseExpression();
            if(rhExpr.has_value())
            {
                rhs = new NodeExpression { .variant = rhExpr.value().variant };
            }

            if(lhs == nullptr || rhs == nullptr)
            {
                return std::nullopt;
            }

            return NodeExpression { .variant = NodeExpressionAdd { .lhs = lhs, .rhs = rhs }};
        }

        return std::nullopt;
    }

    std::optional<NodeExpressionIntLit> Parser::tryParseExpressionIntLit(int offset)
    {
        if (ifType(offset, TokenType::literal_int))
        {
            auto expr = NodeExpressionIntLit {.value = peek(offset).value().value.value()};

            return expr;
        }

        return std::nullopt;
    }

    std::optional<NodeExpressionIdentifier> Parser::tryParseExpressionIdentifier(int offset)
    {
        if(ifType(offset, TokenType::identifier) && !ifType(TokenType::operator_add))
        {
            auto expr = NodeExpressionIdentifier {.value = peek(offset).value().value.value()};

            return expr;
        }

        return std::nullopt;
    }

    std::optional<NodeStatementExit> Parser::parseExit()
    {
        // Expects current token to be of type builtin_exit
        if (!peek().has_value() || peek().value().type != TokenType::builtin_exit)
            return std::nullopt;

        // Expect next token to be of type parenthesis_open
        consume();
        if(!peek().has_value() || peek().value().type != TokenType::parenthesis_open)
            return std::nullopt;

        // Expect next token to be of type expression
        consume();
        std::optional<NodeExpression> expression = parseExpression();

        if(!expression.has_value())
            return std::nullopt;

        if(!peek().has_value() || peek().value().type != TokenType::parenthesis_close)
            return std::nullopt;

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::semicolon)
            return std::nullopt;

        return NodeStatementExit {.expression = expression.value()};
    }

    std::optional<NodeStatementLet> Parser::parseLet()
    {
        if(!peek().has_value() || peek().value().type != TokenType::let)
            return std::nullopt;

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::identifier)
            return std::nullopt;

        std::string identifier = peek().value().value.value();

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::equal_single)
            return std::nullopt;

        consume();
        std::optional<NodeExpression> expression = parseExpression();
        if(!expression.has_value())
            return std::nullopt;

        if(!peek().has_value() || peek().value().type != semicolon)
            return std::nullopt;

        consume();
        return NodeStatementLet {.identifier = identifier, .expression = expression.value()};
    }
}
