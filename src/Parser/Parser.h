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
        std::optional<Tokenizer::Token> consume();

        std::optional<NodeExpression> parseExpression();
        std::optional<NodeStatementExit> parseExit();
    };
}
