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

    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

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
