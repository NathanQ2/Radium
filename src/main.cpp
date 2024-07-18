#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Generator/Generator.h"
#include "Parser/Parser.h"
#include "Tokenizer/Tokenizer.h"

using namespace Radium;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "ERR: Expected 2 arguments but got " << argc << std::endl;

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

    Tokenizer::Tokenizer tokenizer(source);
    std::vector<Tokenizer::Token> tokens = tokenizer.tokenize();

    Parser::Parser parser(tokens);
    NodeRoot root = parser.parse();

    Generator::Generator generator(root);
    std::string output = generator.generate();
    std::cout << "RESULT:\n" << output << std::endl;
    std::ofstream ofs("a.asm");
    ofs << output;
    ofs.close();

    return EXIT_SUCCESS;
}
