#pragma once

#include <vector>

#include "../Tokenizer/Token.h"
#include "Nodes/Nodes.h"

using namespace Radium::Parser::Nodes;

namespace Radium::Parser
{
    class Parser {
    public:
        explicit Parser(std::vector<Tokenizer::Token> tokens);

        NodeRoot parse();
    private:
        std::vector<Tokenizer::Token> m_tokens;
        size_t m_index{};

        std::optional<Tokenizer::Token> peek(int offset = 0);
        std::optional<Tokenizer::Token> consume(int offset = 1);

        bool ifType(int offset, Tokenizer::TokenType type);
        bool ifType(Tokenizer::TokenType type) { return ifType(0, type); }


        std::optional<NodeExpression> parseExpression(int offset = 0);
        std::optional<NodeExpressionIntLit> tryParseExpressionIntLit(int offset = 0);
        std::optional<NodeExpressionIdentifier> tryParseExpressionIdentifier(int offset = 0);
        std::optional<NodeStatementExit> parseExit();
        std::optional<NodeStatementLet> parseLet();
    };
}
