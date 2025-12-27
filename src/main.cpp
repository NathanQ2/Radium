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

// std::string_view tokenTypeToString(TokenType type)
// {
//     switch (type)
//     {
//     case TokenType::parenthesis_open:  return "parenthesis_open";
//     case TokenType::parenthesis_close: return "parenthesis_close";
//     case TokenType::let :              return "let";
//     case TokenType::identifier:        return "identifier";
//     case TokenType::equal_single:      return "equal_single";
//     case TokenType::semicolon:         return "semicolon";
//     case TokenType::literal_int:       return "literal_int";
//     case TokenType::plus:              return "operator_add";
//     case TokenType::minus:             return "operator_subtract";
//     case TokenType::func:              return "func";
//     case TokenType::ret:               return "ret";
//     case TokenType::comma:             return "comma";
//     case TokenType::_if:               return "if";
//     case TokenType::curly_open:        return "curly_open";
//     case TokenType::curly_close:       return "curly_close";
//     case TokenType::mod:               return "mod";
//     case TokenType::include:           return "include";
//     }
//     
//     return "INVALID";
// }

// TODO: Make better way of determining the arena size 
#define ARENA_SIZE 1024

int main(int argc, char* argv[])
{
    Logger::init();
    // if (argc != 2)
    // {
    //     RA_ERROR("Expected 2 arguments but got {0}", argc);

    //     return EXIT_FAILURE;
    // }
    
    CompilationUnit cu = CompilationUnit();
    
    ArenaAllocator arena = ArenaAllocator(ARENA_SIZE);
     
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

        Tokenizer tokenizer(source);
        std::vector<Token> tokens = tokenizer.tokenize();

        // std::cout << "Tokens:" << std::endl;
        // for (const auto& token : tokens) {
        //     std::cout << "("<< tokenTypeToString(token.type);
        //     if (token.value.has_value()) std::cout << ", " << token.value.value();
        //     std::cout << ")";
        // }
        // std::cout << std::endl;
        
        Parser parser(tokens, arena);
        NodeRoot root = parser.parse();
        if (root.modDecl == nullptr)
        {
            RA_ERROR("No module declaration for '{0}'", argv[i]);
            exit(1);
        }
        Module mod = Module(root.modDecl->modPath, root);
        
        cu.addModule(mod);
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
