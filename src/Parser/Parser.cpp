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
                    RA_ERROR("Failed to parse exit statement!");
                    exit(EXIT_FAILURE);
                }

                break;
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

    std::optional<Tokenizer::Token> Parser::consume(int offset)
    {
        m_index += offset;
        return m_tokens.at(m_index);
    }

    bool Parser::ifType(int offset, Tokenizer::TokenType type)
    {
        return peek(offset).has_value() && peek(offset).value().type == type;
    }

    std::optional<Term> Parser::parseTerm()
    {
        if(auto intLit = parseTermIntLit())
        {
            consume();
            return std::optional<Term>(intLit);
        }
        if(auto ident = parseTermIdentifier())
        {
            consume();
            return std::optional<Term>(ident);
        }

        return std::nullopt;
    }

    std::optional<NodeExpression> Parser::parseExpression(const int minPrecedence)
    {
        std::optional<Term> lhs = parseTerm();
        if (!lhs.has_value())
            return std::nullopt;
        while(true)
        {
            std::optional<Tokenizer::Token> curToken = peek();
            if(!curToken.has_value() ||
                !Tokenizer::Token::isBinOperator(curToken.value().type) ||
                !Tokenizer::Token::binPrecedence(curToken.value().type).value() < minPrecedence)
            {
                break;
            }

            Tokenizer::Token op = curToken.value();
            auto prec = Tokenizer::Token::binPrecedence(curToken.value().type);
            if (!prec.has_value())
                return std::nullopt;
            int nextPrec = prec.value() + 1;

            consume();
            auto rhs = parseExpression(nextPrec);

            switch (op.type)
            {
            case Tokenizer::bin_operator_add:
                return NodeExpression {
                    .variant = NodeExpressionAdd {
                        .lhs = new NodeExpression {
                            .variant = lhs.value()
                        },
                        .rhs = new NodeExpression {
                            .variant = rhs.value().variant
                        }
                    }
                };
            case Tokenizer::bin_operator_mult:
                return NodeExpression {
                    .variant = NodeExpressionMult {
                        .lhs = new NodeExpression {
                            .variant = lhs.value()
                        },
                        .rhs = new NodeExpression {
                            .variant = rhs.value().variant
                        }
                    }
                };
            default:
                return std::nullopt;
            }
        }


        // end of bin expr
        // consume();
        return NodeExpression {.variant = lhs.value()};

        // if (std::optional<Term> lhs = parseTerm(); !lhs.has_value())
        //     return std::nullopt;
        //
        // std::optional<NodeExpression> rhs;
        //
        // while (peek().has_value() && Tokenizer::Token::isBinOperator(peek().value().type))
        // {
        //     Tokenizer::Token op = peek().value();
        //     if (Tokenizer::Token::binPrecedence(op.type).value() > minPrecedence)
        //     {
        //
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
    }

    std::optional<TermIntLit> Parser::parseTermIntLit(int offset)
    {
        if (ifType(offset, Tokenizer::TokenType::literal_int))
        {
            auto expr = TermIntLit {.value = peek(offset).value().value.value()};

            return expr;
        }

        return std::nullopt;
    }

    std::optional<TermIdentifier> Parser::parseTermIdentifier(int offset)
    {
        if(ifType(offset, Tokenizer::TokenType::identifier) && !ifType(Tokenizer::TokenType::bin_operator_add))
        {
            auto expr = TermIdentifier {.value = peek(offset).value().value.value()};

            return expr;
        }

        return std::nullopt;
    }

    std::optional<NodeStatementExit> Parser::parseExit()
    {
        // Expects current token to be of type builtin_exit
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
