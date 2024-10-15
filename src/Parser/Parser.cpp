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
            if(peek().value().type == builtin_exit)
            {
                if (auto exitStatement = parseExit())
                {
                    statements.emplace_back(std::move(exitStatement.value()));
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
                    statements.emplace_back(std::move(letStatement.value()));
                }
                else
                {
                    RA_ERROR("Failed to parse variable declaration!");
                    exit(EXIT_FAILURE);
                }
            }
        }

        return NodeRoot{ .statements = std::move(statements) };
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

    std::optional<std::unique_ptr<NodeExpression>> Parser::parseExpression(int minPrecedence)
    {
        std::optional<std::unique_ptr<NodeExpression>> lhs = parseTerm();
        if (!lhs.has_value()) return std::nullopt;

        while (auto tkn = peek())
        {
            std::optional<int> prec = tkn.value().getPrecedence();
            if (!prec.has_value() || prec.value() < minPrecedence)
                return lhs;

            int nextMinPrec = prec.value();
            std::optional<BinaryOpAssociativity> associativity = tkn.value().getAssociativity();
            if (associativity.has_value() && associativity.value() == Left)
            {
                nextMinPrec++;
            }

            consume();
            std::optional<std::unique_ptr<NodeExpression>> rhs = parseExpression(nextMinPrec);

            switch (tkn.value().type)
            {
            case operator_add:
                return std::make_unique<NodeExpression>(
                    std::make_unique<NodeExpressionAdd>(
                        .lhs = std::move(lhs.value()),
                        .rhs = std::move(rhs.value())
                    )
                );
            default:
                return std::nullopt;
            }
        }

        return std::nullopt;
    }

    std::optional<std::unique_ptr<NodeExpression>> Parser::parseTerm()
    {
        std::optional<std::unique_ptr<NodeExpressionIntLit>> intLit = parseExpressionIntLit();
        if (intLit.has_value())
        {
            consume();
            return std::make_unique<NodeExpression>(std::move(intLit.value()));
        }

        std::optional<std::unique_ptr<NodeExpressionIdentifier>> ident = parseExpressionIdentifier();
        if (ident)
        {
            consume();
            return std::make_unique<NodeExpression>(std::move(ident.value()));
        }

        return std::nullopt;
    }

    std::optional<std::unique_ptr<NodeExpressionIntLit>> Parser::parseExpressionIntLit()
    {
        if (ifType(literal_int))
        {
            return std::make_unique<NodeExpressionIntLit>(peek().value().value.value());
        }

        return std::nullopt;
    }

    std::optional<std::unique_ptr<NodeExpressionIdentifier>> Parser::parseExpressionIdentifier()
    {
        if(ifType(identifier) && !ifType(operator_add))
        {
            return std::make_unique<NodeExpressionIdentifier>(peek().value().value.value());
        }

        return std::nullopt;
    }

    std::optional<std::unique_ptr<NodeStatementExit>> Parser::parseExit()
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
        std::optional<std::unique_ptr<NodeExpression>> expression = parseExpression();

        if(!expression.has_value())
            return std::nullopt;

        if(!peek().has_value() || peek().value().type != TokenType::parenthesis_close)
            return std::nullopt;

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::semicolon)
            return std::nullopt;

        return std::make_unique<NodeStatementExit>(expression.value());
    }

    std::optional<std::unique_ptr<NodeStatementLet>> Parser::parseLet()
    {
        if(!peek().has_value() || peek().value().type != TokenType::let)
        {
            RA_ERROR("Not a 'let' statement! token index: {0}", m_index);
            return std::nullopt;
        }

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::identifier)
        {
            RA_ERROR("Not a identifier! token index: {0}", m_index);
            return std::nullopt;
        }

        std::string identifier = peek().value().value.value();

        consume();
        if(!peek().has_value() || peek().value().type != TokenType::equal_single)
        {
            RA_ERROR("Not a '='! token index: {0}", m_index);
            return std::nullopt;
        }

        consume();
        std::optional<std::unique_ptr<NodeExpression>> expression = parseExpression();
        if(!expression.has_value())
        {
            RA_ERROR("Failed to parse expression! token index: {0}", m_index);
            return std::nullopt;
        }

        if(!peek().has_value() || peek().value().type != semicolon)
        {
            RA_ERROR("Not a ';'! token index: {0}", m_index);
            return std::nullopt;
        }

        consume();
        return std::make_unique<NodeStatementLet>(identifier, expression.value());
    }
}
