#include "Parser.h"

#include <codecvt>
#include <iostream>

#include "../Logger/Logger.h"

namespace Radium
{
    Parser::Parser(const std::vector<Token>& tokens) : m_reader(tokens)
    {}

    NodeProgram Parser::parse()
    {
        NodeProgram program;
        program.functions = std::vector<NodeFunctionDecl*>();
        while (m_reader.peek().has_value())
        {
            program.functions.push_back(parseFunctionDecl());
        }

        return program;
    }

    NodeFunctionDecl* Parser::parseFunctionDecl()
    {
        NodeFunctionDecl* decl = new NodeFunctionDecl;
        if (m_reader.peek().value().type != func) RA_ERROR("Expected 'func'");
        m_reader.consume();
        if (m_reader.peek().value().type != identifier) RA_ERROR("Expected ident");
        
        decl->identifier = new NodeIdentifier { m_reader.consume().value.value() };
        
        if (m_reader.peek().value().type != parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        if (m_reader.peek().value().type != parenthesis_close)
            RA_ERROR("Expected ')'");
        m_reader.consume();
        
        NodeBlock* block = parseBlock();
        decl->block = block;

        return decl;
    }

    NodeBlock* Parser::parseBlock()
    {
        if (m_reader.peek().value().type != curly_open) RA_ERROR("Expected '{");
        m_reader.consume();
        
        NodeBlock* block = new NodeBlock;

        block->statements = std::vector<NodeStatement*>();

        while (m_reader.peek().value().type != curly_close)
        {
            NodeStatement* statement = parseStatement();
            block->statements.push_back(statement);
        }
        m_reader.consume();

        return block;
    }

    NodeStatement* Parser::parseStatement()
    {
        NodeStatement* statement = new NodeStatement;
        
        switch (m_reader.peek()->type)
        {
        case let:
            statement->stmt = parseVarDecl();
            break;
        case ret:
            statement->stmt = parseReturn();
            break;
        case _if:
            statement->stmt = parseIf();
            break;
        case curly_open:
            statement->stmt = parseBlock();
            break;
        default:
            NodeExpressionStatement* expr = parseExpressionStatement();
            if (expr == nullptr)
            {
                RA_ERROR("Could not parse statement!");
                exit(1);
            }
            
            statement->stmt = expr;
            break;
        }

        m_reader.consume();
        return statement;
    }

    NodeVarDecl* Parser::parseVarDecl()
    {
        if (m_reader.peek().value().type != let) RA_ERROR("Expected 'let'");
        m_reader.consume();
        if (m_reader.peek().value().type != identifier) RA_ERROR("Expected ident");
        NodeVarDecl* decl = new NodeVarDecl;
        decl->assignment = parseAssignment();

        return decl;
    }

    NodeReturnStatement* Parser::parseReturn()
    {
        if (m_reader.peek().value().type != ret) RA_ERROR("Expected 'ret'");
        m_reader.consume();

        NodeReturnStatement* ret = new NodeReturnStatement;
        ret->expression = parseExpression();

        return ret;
    }

    NodeExpressionStatement* Parser::parseExpressionStatement()
    {
        NodeExpressionStatement* expr = new NodeExpressionStatement;
        
        expr->expression = parseExpression();

        return expr;
    }

    NodeIfStatement* Parser::parseIf()
    {
        NodeIfStatement* nodeIf = new NodeIfStatement;
        
        if (m_reader.peek().value().type != _if)
            RA_ERROR("Expected 'if'");
        m_reader.consume();
        if (m_reader.peek().value().type != parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        
        nodeIf->expr = parseExpression();
        
        if (m_reader.peek().value().type != parenthesis_close)
            RA_ERROR("Expected ')'");
        m_reader.consume();

        nodeIf->block = parseBlock();

        return nodeIf;
    }

    NodeExpression* Parser::parseExpression()
    {
        NodeExpression* expression = new NodeExpression;
        if (m_reader.peek().value().type == identifier && m_reader.peek(1).has_value() && m_reader.peek(1).value().type == equal_single)
        {
            expression->expr = parseAssignment();
        }
        else
        {
            expression->expr = parseAdditive();
        }

        return expression;
    }

    NodeAssignment* Parser::parseAssignment()
    {
        if (m_reader.peek().value().type != identifier) RA_ERROR("Expected ident");
        NodeIdentifier* ident = new NodeIdentifier { m_reader.consume().value.value() };
        if (m_reader.peek().value().type != equal_single) RA_ERROR("Expected '='");
        m_reader.consume();
        
        NodeExpression* expression = parseExpression();

        return new NodeAssignment {std::pair(ident, expression) };
    }

    NodeAdditive* Parser::parseAdditive()
    {
        NodeAdditive* additive = new NodeAdditive;
        additive->left = parseMultiplicative();
        if (m_reader.peek().value().type == operator_add)
        {
            m_reader.consume();
            additive->right = parseMultiplicative();
        }
        else if (m_reader.peek().value().type == operator_subtract)
        {
            additive->isSubtraction = true;
            
            m_reader.consume();
            additive->right = parseMultiplicative();
        }
        else
        {
            additive->right = nullptr;
        }

        return additive;
    }

    NodeMultiplicative* Parser::parseMultiplicative()
    {
        NodeMultiplicative* multiplicative = new NodeMultiplicative;
        multiplicative->primary = parsePrimary();

        return multiplicative;
    }

    NodePrimary* Parser::parsePrimary()
    {
        // Number
        if (m_reader.peek().value().type == literal_int)
        {
            return new NodePrimary { parseNumber() };
        }
        // Ident
        if (m_reader.peek().value().type == identifier && m_reader.peek(1).value().type != parenthesis_open)
        {
            return new NodePrimary { parseIdentifier() };
        }
        // Call
        if (m_reader.peek().value().type == identifier && m_reader.peek(1).value().type == parenthesis_open)
        {
            return new NodePrimary { parseCall() };
        }

        RA_ERROR("Expected primary");
        return nullptr;
    }

    NodeNumber* Parser::parseNumber()
    {
        if (m_reader.peek().value().type != literal_int) RA_ERROR("Expected int literal.");

        return new NodeNumber { m_reader.consume().value.value() };
    }
    
    NodeIdentifier* Parser::parseIdentifier()
    {
        if (m_reader.peek().value().type != identifier) RA_ERROR("Expected ident.");

        return new NodeIdentifier { m_reader.consume().value.value() };
    }

    NodeCall* Parser::parseCall()
    {
        NodeCall* call = new NodeCall;
        call->identifier = parseIdentifier();
        
        if (m_reader.peek().value().type != parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        
        if (m_reader.peek().value().type != parenthesis_close)
        {
            call->argList = parseArgList();
        }
        m_reader.consume();

        return call;
    }

    NodeArgList* Parser::parseArgList()
    {
        NodeArgList* argList = new NodeArgList;
        argList->args = std::vector<NodeExpression*>();

        argList->args.push_back(parseExpression());
        while (m_reader.peek().value().type == comma)
        {
            m_reader.consume();
            argList->args.push_back(parseExpression());
        }
        
        return argList;
    }
}
