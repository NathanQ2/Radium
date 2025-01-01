#include "Parser.h"

#include <codecvt>
#include <iostream>

#include "../Logger/Logger.h"

namespace Radium
{
    Parser::Parser(const std::vector<Token>& tokens) : m_reader(tokens)
    {}

    NodeRoot Parser::parse()
    {
        std::vector<NodeFunction*> functions;
        while (m_reader.peek().has_value())
        {
            if (const auto func = parseFunction())
            {
                functions.emplace_back(func.value());
            }
        }

        return NodeRoot { .functions = std::move(functions) };
    }

    std::optional<NodeFunction*> Parser::parseFunction()
    {
        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == func; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == identifier; }))
            return std::nullopt;
        std::string ident = m_reader.consume().value.value();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == parenthesis_open; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == parenthesis_close; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == curly_open; }))
            return std::nullopt;
        m_reader.consume();
        
        std::vector<NodeStatement> statements;
        std::vector<Token> stmnt = std::vector<Token>();
        int curlyDelta = 1;
        while(m_reader.peek().has_value())
        {
            if (m_reader.peekAnd([](const Token& tkn) { return tkn.type == curly_open; }))
                curlyDelta++;
            if (m_reader.peekAnd([](const Token& tkn) { return tkn.type == curly_close; }))
                curlyDelta--;

            if (curlyDelta == 0)
                break;
            
            std::optional<NodeStatement> statement = parseStatement();
            if (!statement.has_value())
            {
                RA_ERROR("Failed to parse statement!");

                exit(EXIT_FAILURE);
            }
            
            stmnt.clear();
            statements.push_back(statement.value());
        }

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == curly_close; }))
            return std::nullopt;
        m_reader.consume();

        return new NodeFunction{ .statements = std::move(statements), .identifier = ident };
    }

    std::optional<NodeExpression*> Parser::parseExpression(int minPrecedence)
    {
        NodeExpression* lhs = nullptr;
        if (const auto l = parseAtom())
        {
            lhs = l.value();
        }
        else
        {
            return std::nullopt;
        }

        while (true)
        {
            if (m_reader.peekAnd([minPrecedence](const auto& tkn)
            {
                return tkn.isBinOp() && tkn.getPrecedence() <= minPrecedence;
            }))
            {
                Token binOp = m_reader.peek().value();
                int precedence = binOp.getPrecedence().value();
                BinaryOpAssociativity assoc = binOp.getAssociativity().value();
                int nextMinPrecedence = assoc == Left ? precedence + 1 : precedence;

                // m_reader.consumeUntil([](const auto& tkn) { return !tkn.isBinOp(); });
                m_reader.consume();
                NodeExpression* rhs = nullptr;
                if (auto r = parseExpression(nextMinPrecedence))
                {
                    rhs = r.value();
                }

                switch (binOp.type)
                {
                case operator_add:
                    return new NodeExpression { .variant = new NodeExpressionAdd { .lhs = lhs, .rhs = rhs } };
                default:
                    return std::nullopt;
                }
            }
            else
            {
                break;
            }

            // if (m_reader.peekAnd([](const auto& tkn) { return tkn.type == semicolon || tkn.type == parenthesis_close; }))
            //     break;
            //
            // m_reader.consume();
        }

        return lhs;
    }

    std::optional<NodeExpression*> Parser::parseAtom()
    {
        if (auto intLit = parseExpressionIntLit())
        {
            return new NodeExpression { .variant = new NodeAtom { .variant = intLit.value() } };
        }

        if (auto ident = parseExpressionIdentifier())
        {
            return new NodeExpression { .variant = new NodeAtom { .variant = ident.value() } };
        }

        if (m_reader.peekAnd([](const auto& tkn) { return tkn.type == parenthesis_open; }))
        {
            // m_reader.consumeUntil([](const auto& tkn) { return tkn.isAtom(); });
            if (auto expr = parseExpression())
            {
                if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == parenthesis_close; }))
                {
                    RA_ERROR("No closing ')'!");
                }
                
                m_reader.consume();
                return expr;
            }

            return std::nullopt;
        }

        return std::nullopt;
    }

    std::optional<NodeExpressionIntLit*> Parser::parseExpressionIntLit()
    {
        std::stringstream ss;
        while (m_reader.peekAnd([](const auto& tkn) { return tkn.type == literal_int; }))
        {
            ss << m_reader.consume().value.value();
        }

        if (ss.str().length() == 0)
            return std::nullopt;

        return new NodeExpressionIntLit { .value = ss.str() };
    }

    std::optional<NodeExpressionIdentifier*> Parser::parseExpressionIdentifier()
    {
        if (m_reader.peekAnd([](const auto& tkn) { return tkn.type == identifier; }))
        {
            return new NodeExpressionIdentifier { .value = m_reader.consume().value.value() };
        }

        return std::nullopt;
    }

    std::optional<NodeStatementExit*> Parser::parseExit()
    {
        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == builtin_exit; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == parenthesis_open; }))
            return std::nullopt;
        m_reader.consume();

        NodeExpression* expression = nullptr;
        if (auto expr = parseExpression())
        {
            expression = expr.value();
        }
        else
        {
            return std::nullopt;
        }

        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == parenthesis_close; }))
            return std::nullopt;

        m_reader.consume();
        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == semicolon; }))
            return std::nullopt;

        return new NodeStatementExit { .exitCode = expression };
    }

    std::optional<NodeStatementLet*> Parser::parseLet()
    {
        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == let; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == identifier; }))
            return std::nullopt;
        
        std::string ident = m_reader.consume().value.value();

        if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == equal_single; }))
            return std::nullopt;
        m_reader.consume();

        if (auto expr = parseExpression())
        {
            if (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == semicolon; }))
                return std::nullopt;
            
            return new NodeStatementLet { .identifier = ident, .value = expr.value() };
        }

        return std::nullopt;
    }

    std::optional<NodeStatementFunctionCall*> Parser::parseFunctionCall()
    {
        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == identifier; }))
            return std::nullopt;
        std::string ident = m_reader.consume().value.value();
        
        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == parenthesis_open; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == parenthesis_close; }))
            return std::nullopt;
        m_reader.consume();

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == semicolon; }))
            return std::nullopt;
        m_reader.consume();

        return new NodeStatementFunctionCall {
            .identifier = ident
        };
    }

    std::optional<NodeStatementRet*> Parser::parseRet()
    {
        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == ret; }))
            return std::nullopt;
        m_reader.consume();

        NodeExpression* expr;
        if (auto expression = parseExpression())
        {
            expr = expression.value();
        }
        else
        {
            return std::nullopt;
        }

        if (!m_reader.peekAnd([](const Token& tkn) { return tkn.type == semicolon; }))
            return std::nullopt;
        m_reader.consume();

        return new NodeStatementRet {
            .value = expr
        };
    }


    std::optional<NodeStatement> Parser::parseStatement()
    {
        while (!m_reader.peekAnd([](const auto& tkn) { return tkn.type == semicolon; }))
        {
            Token tkn = m_reader.peek().value();

            if (auto let = parseLet())
            {
                m_reader.consume();
                return NodeStatement {
                    .variant = let.value()
                };
            }
            
            if (auto exit = parseExit())
            {
                m_reader.consume();
                return NodeStatement {
                    .variant = exit.value()
                };
            }

            if (auto functionCall = parseFunctionCall())
            {
                return NodeStatement {
                    .variant = functionCall.value()
                };
            }

            if (auto ret = parseRet())
            {
                return NodeStatement {
                    .variant = ret.value()
                };
            }
        }

        return std::nullopt;
    }
}
