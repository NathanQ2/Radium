#include "Parser.h"

#include <codecvt>
#include <iostream>

#include "../Logger/Logger.h"


namespace Radium
{
    Parser::Parser(const std::vector<Token>& tokens, ArenaAllocator& alloc) : m_reader(tokens), m_alloc(alloc)
    {}

    NodeRoot Parser::parse()
    {
        NodeRoot program {
            .modDecl = nullptr,
            .includes = std::vector<NodeModuleInclude*>(),
            .functions = std::vector<NodeFunctionDecl*>()
        };
        
        while (m_reader.peek().has_value())
        {
            switch (m_reader.peek().value().type)
            {
            case TokenType::mod:
                program.modDecl = parseModuleDecl();
                
                break;
            case TokenType::include:
                program.includes.push_back(parseModuleInclude());
                
                break;
            case TokenType::func:
                program.functions.push_back(parseFunctionDecl());
                
                break;
            case TokenType::eof:
                return program;
            default:
                RA_ERROR("Invalid token in module body");
                
                break;
            }
        }

        return program;
    }

    NodeModuleDecl* Parser::parseModuleDecl()
    {
        NodeModuleDecl* decl = m_alloc.alloc<NodeModuleDecl>();
        
        if (m_reader.peek().value().type != TokenType::mod) 
            RA_ERROR("Expected 'mod'");
        m_reader.consume();
        
        if (m_reader.peek().value().type != TokenType::identifier)
            RA_ERROR("Expected identifier!");
        decl->modPath = m_reader.consume().value.value();
        
        if (m_reader.peek().value().type != TokenType::semicolon)
            RA_ERROR("Expected ';'");
        m_reader.consume();
        
        return decl;
    }

    NodeModuleInclude* Parser::parseModuleInclude()
    {
        NodeModuleInclude* include = m_alloc.alloc<NodeModuleInclude>();
        if (m_reader.peek().value().type != TokenType::include) 
            RA_ERROR("Expected 'include'");
        m_reader.consume();
        
        if (m_reader.peek().value().type != TokenType::identifier)
            RA_ERROR("Expected identifier!");
        include->modPath = m_reader.consume().value.value();
        
        if (m_reader.peek().value().type != TokenType::semicolon)
            RA_ERROR("Expected ';'");
        m_reader.consume();
        
        return include;
    }

    NodeFunctionDecl* Parser::parseFunctionDecl()
    {
        NodeFunctionDecl* decl = m_alloc.alloc<NodeFunctionDecl>();
        if (m_reader.peek().value().type != TokenType::func) RA_ERROR("Expected 'func'");
        m_reader.consume();
        if (m_reader.peek().value().type != TokenType::identifier) RA_ERROR("Expected ident");
        
        NodeIdentifier* ident = m_alloc.alloc<NodeIdentifier>();
        ident->value = m_reader.consume().value.value();
        
        decl->identifier = ident;
        
        if (m_reader.peek().value().type != TokenType::parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        if (m_reader.peek().value().type != TokenType::parenthesis_close)
            RA_ERROR("Expected ')'");
        m_reader.consume();
        
        NodeBlock* block = parseBlock();
        decl->block = block;

        return decl;
    }

    NodeBlock* Parser::parseBlock()
    {
        if (m_reader.peek().value().type != TokenType::curly_open) RA_ERROR("Expected '{");
        m_reader.consume();
        
        NodeBlock* block = m_alloc.alloc<NodeBlock>();

        block->statements = std::vector<NodeStatement*>();

        while (m_reader.peek().value().type != TokenType::curly_close)
        {
            NodeStatement* statement = parseStatement();
            block->statements.push_back(statement);
        }
        m_reader.consume();

        return block;
    }

    NodeStatement* Parser::parseStatement()
    {
        NodeStatement* statement = m_alloc.alloc<NodeStatement>();
        
        switch (m_reader.peek()->type)
        {
        case TokenType::let:
            statement->stmt = parseVarDecl();
            break;
        case TokenType::ret:
            statement->stmt = parseReturn();
            break;
        case TokenType::_if:
            statement->stmt = parseIf();
            break;
        case TokenType::curly_open:
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
        if (m_reader.peek().value().type != TokenType::let) RA_ERROR("Expected 'let'");
        m_reader.consume();
        if (m_reader.peek().value().type != TokenType::identifier) RA_ERROR("Expected ident");
        NodeVarDecl* decl = m_alloc.alloc<NodeVarDecl>();
        decl->assignment = parseAssignment();

        return decl;
    }

    NodeReturnStatement* Parser::parseReturn()
    {
        if (m_reader.peek().value().type != TokenType::ret) RA_ERROR("Expected 'ret'");
        m_reader.consume();

        NodeReturnStatement* ret = m_alloc.alloc<NodeReturnStatement>();
        ret->expression = parseExpression();

        return ret;
    }

    NodeExpressionStatement* Parser::parseExpressionStatement()
    {
        NodeExpressionStatement* expr = m_alloc.alloc<NodeExpressionStatement>();
        
        expr->expression = parseExpression();

        return expr;
    }

    NodeIfStatement* Parser::parseIf()
    {
        NodeIfStatement* nodeIf = m_alloc.alloc<NodeIfStatement>();
        
        if (m_reader.peek().value().type != TokenType::_if)
            RA_ERROR("Expected 'if'");
        m_reader.consume();
        if (m_reader.peek().value().type != TokenType::parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        
        nodeIf->expr = parseExpression();
        
        if (m_reader.peek().value().type != TokenType::parenthesis_close)
            RA_ERROR("Expected ')'");
        m_reader.consume();

        nodeIf->block = parseBlock();

        return nodeIf;
    }

    NodeExpression* Parser::parseExpression()
    {
        NodeExpression* expression = m_alloc.alloc<NodeExpression>();
        if (m_reader.peek().value().type == TokenType::identifier && m_reader.peek(1).has_value() && m_reader.peek(1).value().type == TokenType::equal_single)
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
        if (m_reader.peek().value().type != TokenType::identifier) RA_ERROR("Expected ident");
        NodeIdentifier* ident = new NodeIdentifier { m_reader.consume().value.value() };
        if (m_reader.peek().value().type != TokenType::equal_single) RA_ERROR("Expected '='");
        m_reader.consume();
        
        NodeExpression* expression = parseExpression();

        return new NodeAssignment {std::pair(ident, expression) };
    }

    NodeAdditive* Parser::parseAdditive()
    {
        NodeAdditive* additive = new NodeAdditive;
        additive->left = parseMultiplicative();
        TokenType operator_add;
        if (m_reader.peek().value().type == TokenType::plus)
        {
            m_reader.consume();
            additive->right = parseMultiplicative();
        }
        else if (m_reader.peek().value().type == TokenType::minus)
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
        if (m_reader.peek().value().type == TokenType::literal_int)
        {
            return new NodePrimary { parseNumber() };
        }
        // Ident
        if (m_reader.peek().value().type == TokenType::identifier && m_reader.peek(1).value().type != TokenType::parenthesis_open)
        {
            return new NodePrimary { parseIdentifier() };
        }
        // Call
        if (m_reader.peek().value().type == TokenType::identifier && m_reader.peek(1).value().type == TokenType::parenthesis_open)
        {
            return new NodePrimary { parseCall() };
        }

        RA_ERROR("Expected primary");
        return nullptr;
    }

    NodeNumber* Parser::parseNumber()
    {
        if (m_reader.peek().value().type != TokenType::literal_int) RA_ERROR("Expected int literal.");

        return new NodeNumber { m_reader.consume().value.value() };
    }
    
    NodeIdentifier* Parser::parseIdentifier()
    {
        if (m_reader.peek().value().type != TokenType::identifier) RA_ERROR("Expected ident.");

        return new NodeIdentifier { m_reader.consume().value.value() };
    }

    NodeCall* Parser::parseCall()
    {
        NodeCall* call = new NodeCall;
        call->identifier = parseIdentifier();
        
        if (m_reader.peek().value().type != TokenType::parenthesis_open)
            RA_ERROR("Expected '('");
        m_reader.consume();
        
        if (m_reader.peek().value().type != TokenType::parenthesis_close)
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
        while (m_reader.peek().value().type == TokenType::comma)
        {
            m_reader.consume();
            argList->args.push_back(parseExpression());
        }
        
        return argList;
    }
}
