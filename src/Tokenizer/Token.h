#pragma once

#include <string>
#include <optional>
#include <utility>

#include "TokenType.h"

namespace Radium
{
    enum BinaryOpAssociativity
    {
        Left,
        Right
    };

    class Token {
    public:
        Token(TokenType type, std::string value)
        {
            this->type = type;
            this->value = value;
        }

        Token(TokenType type)
        {
            this->type = type;
            this->value = std::nullopt;
        }

        TokenType type;
        std::optional<std::string> value;

        [[nodiscard]] static std::optional<int> getPrecedence(const TokenType& type)
        {
            switch (type)
            {
            case operator_add:
                return 1;
            default:
                return std::nullopt;
            }
        }

        [[nodiscard]] static std::optional<BinaryOpAssociativity> getAssociativity(const TokenType& tkn)
        {
            switch(tkn)
            {
            case operator_add:
                return Left;
            default:
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<int> getPrecedence() const
        {
            return getPrecedence(this->type);
        }

        [[nodiscard]] std::optional<BinaryOpAssociativity> getAssociativity() const
        {
            return getAssociativity(this->type);
        }
    };
}
