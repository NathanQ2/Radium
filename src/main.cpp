#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "CompilationUnit.h"
#include "Generator/Generator.h"
#include "Logger/Logger.h"
#include "Parser/Parser.h"
#include "Tokenizer/Tokenizer.h"

using namespace Radium;


std::string_view tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case parenthesis_open:  return "parenthesis_open";
    case parenthesis_close: return "parenthesis_close";
    case let :              return "let";
    case identifier:        return "identifier";
    case equal_single:      return "equal_single";
    case semicolon:         return "semicolon";
    case literal_int:       return "literal_int";
    case operator_add:      return "operator_add";
    case operator_subtract: return "operator_subtract";
    case func:              return "func";
    case ret:               return "ret";
    case comma:             return "comma";
    case _if:               return "if";
    case curly_open:        return "curly_open";
    case curly_close:       return "curly_close";
    case mod:               return "mod";
    case include:           return "include";
    }
    
    return "INVALID";
}

int main(int argc, char* argv[])
{
    Logger::init();
    // if (argc != 2)
    // {
    //     RA_ERROR("Expected 2 arguments but got {0}", argc);

    //     return EXIT_FAILURE;
    // }
    
    CompilationUnit cu = CompilationUnit();
     
    Tokenizer tokenizer = Tokenizer();
    
    // For each source file
    for (int i = 1; i < argc; i++)
    {
        std::string source;
        {
            std::stringstream ss;
            std::ifstream ifs(argv[i]);

            ss << ifs.rdbuf();
            source = ss.str();
            ifs.close();
        }

        std::vector<Token> tokens = tokenizer.tokenize(source);

        // std::cout << "Tokens:" << std::endl;
        // for (const auto& token : tokens) {
        //     std::cout << "("<< tokenTypeToString(token.type);
        //     if (token.value.has_value()) std::cout << ", " << token.value.value();
        //     std::cout << ")";
        // }
        // std::cout << std::endl;
        
        Parser parser(tokens);
        NodeRoot root = parser.parse();
        //root.modDecl = NodeModuleDecl { .modPath = "main" };
        
        cu.addModule(root);
    }
    
    Generator generator(cu);
    std::string output = generator.generate();
    RA_TRACE("Compilation result:\n{0}", output);

    std::ofstream ofs("a.asm");
    ofs << output;
    ofs.close();

    return EXIT_SUCCESS;
}

// Test module
/*
    cu.addModule(NodeRoot {
        .modDecl = new NodeModuleDecl { .modPath = "test" },
        .includes = std::vector<NodeModuleInclude*>(),
        .functions = std::vector { 
            new NodeFunctionDecl { 
                .identifier =  new NodeIdentifier {"test1" }, 
                .block = new NodeBlock { 
                    .statements = std::vector {
                        new NodeStatement {
                        new NodeExpressionStatement {
                            .expression = new NodeExpression {
                                .expr = new NodeAdditive {
                                    .left = new NodeMultiplicative { .primary = new NodePrimary { new NodeCall {
                                        .identifier = new NodeIdentifier { "exit" },
                                        .argList = new NodeArgList {
                                            .args = std::vector {
                                                new NodeExpression { .expr = new NodeAdditive {
                                                    .left = new NodeMultiplicative {
                                                        .primary = new NodePrimary {
                                                            .value = new NodeNumber {
                                                                .value = "10"
                                                            }
                                                        }
                                                    },
                                                    .right = nullptr,
                                                    .isSubtraction = false
                                                }}
                                            }
                                        }
                                    }}},
                                    .right = nullptr,
                                    .isSubtraction = false
                                }
                            }
                        }
                        }
                    }
                }
            }
        }
    });
*/
