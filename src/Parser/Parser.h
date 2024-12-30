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

        [[nodiscard]] std::optional<NodeFunction*> parseFunction();
        [[nodiscard]] std::optional<NodeExpression*> parseExpression(int minPrecedence = 1);
        [[nodiscard]] std::optional<NodeExpression*> parseAtom();
        [[nodiscard]] std::optional<NodeExpressionIntLit*> parseExpressionIntLit();
        [[nodiscard]] std::optional<NodeExpressionIdentifier*> parseExpressionIdentifier();
        [[nodiscard]] std::optional<NodeStatement> parseStatement();
        [[nodiscard]] std::optional<NodeStatementExit*> parseExit();
        [[nodiscard]] std::optional<NodeStatementLet*> parseLet();
    };
}
