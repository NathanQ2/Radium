#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Generator/Generator.h"
#include "Logger/Logger.h"
#include "Parser/Parser.h"
#include "Tokenizer/Tokenizer.h"

using namespace Radium;


std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case parenthesis_open: return "parenthesis_open";
    case parenthesis_close: return "parenthesis_close";
    case let : return "let";
    case identifier: return "identifier";
    case equal_single: return "equal_single";
    case semicolon: return "semicolon";
    case literal_int: return "literal_int";
    case operator_add: return "operator_add";
    case func: return "func";
    case ret: return "ret";
    case curly_open: return "curly_open";
    case curly_close: return "curly_close";
    }
}

int main(int argc, char* argv[])
{
    Logger::init();
    if (argc != 2)
    {
        RA_ERROR("Expected 2 arguments but got {0}", argc);

        return EXIT_FAILURE;
    }

    std::string source;
    {
        std::stringstream ss;
        std::ifstream ifs(argv[1]);

        ss << ifs.rdbuf();
        source = ss.str();
        ifs.close();
    }

    TokenizerConfiguration tokenizerConfig = {
        .tokenizeIntLit = true,
        .tokenizeIdentifier = true,
        .punctuators = { '(', ')', '=', ';', '+', '{', '}' },
        .keywords = { "let", "func", "ret" },
    };
    Tokenizer tokenizer(tokenizerConfig);
    std::vector<Token> tokens = tokenizer.tokenize(source);

    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "("<< tokenTypeToString(token.type);
        if (token.value.has_value()) std::cout << ", " << token.value.value();
        std::cout << ")";
    }
    std::cout << std::endl;

    Parser parser(tokens);
    NodeProgram program = parser.parse();

    Generator generator(program);
    std::string output = generator.generate();
    RA_TRACE("Compilation result:\n{0}", output);

    std::ofstream ofs("a.asm");
    ofs << output;
    ofs.close();

    return EXIT_SUCCESS;
}
