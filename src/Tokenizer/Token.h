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

        [[nodiscard]] static std::optional<TokenType> getTypeFromString(std::string_view str) {
            if (str == "(") return parenthesis_open;
            if (str == ")") return parenthesis_close;
            if (str == "let") return let;
            if (str == "=") return equal_single;
            if (str == ";") return semicolon;
            if (str == "+") return operator_add;
            if (str == "func") return func;
            if (str == "ret") return ret;
            if (str == ",") return comma;
            if (str == "{") return curly_open;
            if (str == "}") return curly_close;

            return std::nullopt;
        }

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

        [[nodiscard]] static std::optional<BinaryOpAssociativity> getAssociativity(const TokenType& type)
        {
            switch(type)
            {
            case operator_add:
                return Left;
            default:
                return std::nullopt;
            }
        }

        [[nodiscard]] static bool isBinOp(const TokenType& type)
        {
            return getPrecedence(type).has_value();
        }

        [[nodiscard]] static bool isAtom(const TokenType& type)
        {
            return type == literal_int || type == identifier;
        }

        [[nodiscard]] std::optional<int> getPrecedence() const
        {
            return getPrecedence(this->type);
        }

        [[nodiscard]] std::optional<BinaryOpAssociativity> getAssociativity() const
        {
            return getAssociativity(this->type);
        }

        [[nodiscard]] bool isBinOp() const
        {
            return isBinOp(this->type);
        }

        [[nodiscard]] bool isAtom() const
        {
            return isAtom(this->type);
        }
    };
}
