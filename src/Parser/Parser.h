#pragma once

#include <vector>

#include "../Reader.h"
#include "../Tokenizer/Token.h"
#include "Nodes/Nodes.h"

using namespace Radium;

namespace Radium
{
    class Parser {
    public:
        explicit Parser(const std::vector<Token>& tokens);

        NodeRoot parse();
    private:
        Reader<Token> m_reader;

        [[nodiscard]] NodeModuleDecl* parseModuleDecl();
        [[nodiscard]] NodeModuleInclude* parseModuleInclude();
        [[nodiscard]] NodeFunctionDecl* parseFunctionDecl();
        [[nodiscard]] NodeBlock* parseBlock();
        
        [[nodiscard]] NodeStatement* parseStatement();
        [[nodiscard]] NodeVarDecl* parseVarDecl();
        [[nodiscard]] NodeReturnStatement* parseReturn();
        [[nodiscard]] NodeExpressionStatement* parseExpressionStatement();
        [[nodiscard]] NodeIfStatement* parseIf();

        [[nodiscard]] NodeExpression* parseExpression();
        [[nodiscard]] NodeAssignment* parseAssignment();
        [[nodiscard]] NodeAdditive* parseAdditive();
        [[nodiscard]] NodeMultiplicative* parseMultiplicative();
        [[nodiscard]] NodePrimary* parsePrimary();
        [[nodiscard]] NodeNumber* parseNumber();
        [[nodiscard]] NodeIdentifier* parseIdentifier();
        [[nodiscard]] NodeCall* parseCall();
        [[nodiscard]] NodeArgList* parseArgList();
    };
}
