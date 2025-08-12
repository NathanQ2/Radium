#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Generator/Generator.h"
#include "Logger/Logger.h"
#include "Parser/Parser.h"
#include "Tokenizer/Tokenizer.h"

using namespace Radium;

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
        .keywords = { "let", "exit", "func", "ret" },
    };
    Tokenizer tokenizer(tokenizerConfig);
    std::vector<Token> tokens = tokenizer.tokenize(source);

    // std::cout << "Tokens:" << std::endl;
    // for (const auto& token : tokens) {
    //     std::cout << "("<< token.type;
    //     if (token.value.has_value()) std::cout << ", " << token.value.value();
    //     std::cout << ")";
    // }
    // std::cout << std::endl;

    Parser parser(tokens);
    NodeRoot root = parser.parse();

    Generator generator(root);
    std::string output = generator.generate();
    RA_TRACE("Compilation result:\n{0}", output);

    std::ofstream ofs("a.asm");
    ofs << output;
    ofs.close();

    return EXIT_SUCCESS;
}
