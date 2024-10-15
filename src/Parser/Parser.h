#pragma once

#include <vector>

#include "../Tokenizer/Token.h"
#include "Nodes/Nodes.h"

using namespace Radium;

namespace Radium
{
    class Parser {
    public:
        explicit Parser(std::vector<Token> tokens);

        NodeRoot parse();
    private:
        std::vector<Token> m_tokens;
        size_t m_index{};

        std::optional<Token> peek(int offset = 0);
        std::optional<Token> consume(int offset = 1);

        bool ifType(int offset, TokenType type);
        bool ifType(TokenType type) { return ifType(0, type); }


        std::optional<std::unique_ptr<NodeExpression>> parseExpression(int minPrecedence = 0);
        std::optional<std::unique_ptr<NodeExpression>> parseTerm();
        [[nodiscard]] std::optional<std::unique_ptr<NodeExpressionIntLit>> parseExpressionIntLit();
        [[nodiscard]] std::optional<std::unique_ptr<NodeExpressionIdentifier>> parseExpressionIdentifier();
        [[nodiscard]] std::optional<std::unique_ptr<NodeStatementExit>> parseExit();
        [[nodiscard]] std::optional<std::unique_ptr<NodeStatementLet>> parseLet();
    };
}
