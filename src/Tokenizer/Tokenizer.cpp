#include "Tokenizer.h"

#include "../Logger/Logger.h"

namespace Radium {
    Tokenizer::Tokenizer(std::string_view source) : m_source(source), m_i(0)
    {
    }

    Token Tokenizer::next() 
    {
        // Skip whitespace
        while (std::isspace(m_source[m_i]) && m_i < m_source.length())
        {
            m_i++;
        }
        
        // If this line is a comment, consume the line and call recursively
        if (m_i + 1 < m_source.length() && m_source[m_i] == '/' && m_source[m_i + 1] == '/')
        {
            while (m_source[m_i] != '\n' && m_i < m_source.length())
                m_i++;
            
            m_i++;
            return next();
        }
        
        if (m_i >= m_source.length())
            return TokenType::eof;
        
        switch (m_source[m_i])
        {
        case '(': 
            m_i++;
            return TokenType::parenthesis_open;
        case ')':
            m_i++;
            return TokenType::parenthesis_close;
        case '=':
            m_i++;
            return TokenType::equal_single;
        case ';': 
            m_i++;
            return TokenType::semicolon;
        case '+':
            m_i++;
            return TokenType::plus;
        case '-': 
            m_i++;
            return TokenType::minus;
        case '{': 
            m_i++;
            return TokenType::curly_open;
        case '}': 
            m_i++;
            return TokenType::curly_close;
        case ',': 
            m_i++;
            return TokenType::comma;
        default:
            // Read int lit
            if (std::isdigit(m_source[m_i]))
            {
                size_t start = m_i;
                while (std::isdigit(m_source[m_i]))
                    m_i++;
                size_t end = m_i;
                
                std::string val = std::string(m_source.substr(start, end - start));
                return Token(TokenType::literal_int, std::move(val));
            }
            // Read word
            if (std::isalpha(m_source[m_i]))
            {
                size_t start = m_i;
                while (std::isalnum(m_source[m_i]))
                    m_i++;
                size_t end = m_i;
                
                std::string word = std::string(m_source.substr(start, end - start));
                
                if (word == "let")     return TokenType::let;
                if (word == "func")    return TokenType::func;
                if (word == "ret")     return TokenType::ret;
                if (word == "mod")     return TokenType::mod;
                if (word == "include") return TokenType::include;    
                if (word == "if")      return TokenType::_if;
                
                return Token(TokenType::identifier, std::move(word));
            }
            
            break;
        }
        
        return TokenType::eof;
    }

    std::vector<Token> Tokenizer::tokenize()
    {
        std::vector<Token> tokens;
        
        while (true)
        {
            Token tkn = next();
        
            tokens.push_back(tkn);
        
            if (tkn.type == TokenType::eof) break;
        }
        
        return tokens;
    }

    // std::vector<Token> Tokenizer::tokenize(std::string_view source) {
    //     std::vector<Token> tokens;
    //     Reader<char> reader(std::vector(source.begin(), source.end()));

    //     std::vector<char> word;
    //     while (reader.peek().has_value()) {
    //         if (reader.peek().value() == '/' && reader.peek(1).has_value() && reader.peek(1).value() == '/') {
    //             reader.consumeUntil([](const char c) { return c == '\n'; });
    //             reader.consume();

    //             continue;
    //         }
    //         
    //         if (std::find(s_punctuators.begin(), s_punctuators.end(), reader.peek().value()) != s_punctuators.end()) {
    //             char c = reader.peek().value();
    //             std::optional<TokenType> type = Token::getTypeFromString(std::string_view(&c, 1));
    //             if (type.has_value()) tokens.emplace_back(type.value());
    //             else RA_ERROR("Found punctuator, {0}, but could not get TokenType", c);

    //             reader.consume();
    //             continue;
    //         }

    //         if (std::isspace(reader.peek().value())) {
    //             word.clear();
    //             reader.consume();

    //             continue;
    //         }

    //         // Begin word
    //         if (std::isalpha(reader.peek().value())) {
    //             word = reader.consumeUntil([this](const char c) { return isSpaceOrPunctuator(c); });

    //             std::optional<TokenType> type = Token::getTypeFromString(std::string_view(word.begin(), word.end()));
    //             // Keyword
    //             if (type.has_value()) {
    //                 tokens.emplace_back(type.value());
    //             }
    //             // Identifier
    //             else {
    //                 tokens.emplace_back(identifier, std::string(word.begin(), word.end()));
    //             }
    //         }
    //         else if (std::isdigit(reader.peek().value())) {
    //             // Assume int lit
    //             word = reader.consumeUntil([](const char c) { return !std::isdigit(c); });

    //             tokens.emplace_back(literal_int, std::string(word.begin(), word.end()));
    //         }
    //     }

    //     return tokens; 
    // }
}
