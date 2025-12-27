#pragma once

#include <string>
#include <optional>
#include <utility>

#include "TokenType.h"
#include "../Logger/Logger.h"

namespace Radium
{
    enum class BinaryOpAssociativity
    {
        Left,
        Right
    };

    class Token {
    public:
        Token(TokenType type, std::string&& value)
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
            if (str == "(") return TokenType::parenthesis_open;
            if (str == ")") return TokenType::parenthesis_close;
            if (str == "let") return TokenType::let;
            if (str == "=") return TokenType::equal_single;
            if (str == ";") return TokenType::semicolon;
            if (str == "+") return TokenType::plus;
            if (str == "-") return TokenType::minus;
            if (str == "func") return TokenType::func;
            if (str == "ret") return TokenType::ret;
            if (str == "mod") return TokenType::mod;
            if (str == "include") return TokenType::include;
            if (str == ",") return TokenType::comma;
            if (str == "if") return TokenType::_if;
            if (str == "{") return TokenType::curly_open;
            if (str == "}") return TokenType::curly_close;
            
            return std::nullopt;
        }

        [[nodiscard]] static std::optional<int> getPrecedence(const TokenType& type)
        {
            switch (type)
            {
            case TokenType::plus:
                return 1;
            default:
                return std::nullopt;
            }
        }

        [[nodiscard]] static std::optional<BinaryOpAssociativity> getAssociativity(const TokenType& type)
        {
            switch(type)
            {
            case TokenType::plus:
                return BinaryOpAssociativity::Left;
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
            return type == TokenType::literal_int || type == TokenType::identifier;
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
