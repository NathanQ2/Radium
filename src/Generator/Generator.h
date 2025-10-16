#pragma once

#include <sstream>
#include <unordered_map>

#include "../Parser/Nodes/Nodes.h"

using namespace Radium;

namespace Radium
{
    class Generator {
    public:
        explicit Generator(NodeProgram program);

        std::string generate();
    private:
        NodeProgram m_program;

        std::stringstream m_ss;

        std::unordered_map<std::string, size_t> m_identifierStackPositions;
        std::unordered_map<std::string, bool> m_registers;
        size_t m_stackSizeBytes;

        void mov(const std::string& dest, const std::string& val);

        void push(const std::string& reg, size_t sizeBytes);
        void pop(const std::string& reg, size_t sizeBytes);

        std::string reserveRegister();
        void freeRegister(const std::string& reg);

        void generateFunction(const NodeFunctionDecl* func);
        void generateBlock(const NodeBlock* block);
        void generateStatement(const NodeStatement* statement);

        void generateVarDecl(const NodeVarDecl* varDecl);
        void generateExpressionStatement(const NodeExpressionStatement* statement);
        void generateReturnStatement(const NodeReturnStatement* ret);

        void generateExpression(const NodeExpression* expr, const std::string& dest);
        void generateAssignment(const NodeAssignment* assignment, const std::string& dest);
        void generateAssignment(std::pair<NodeIdentifier*, NodeExpression*> assignment, const std::string& dest);
        void generateAdditive(const NodeAdditive* additive, const std::string& dest);
        void generateMultiplicative(const NodeMultiplicative* multi, const std::string& dest);
        void generatePrimary(const NodePrimary* primary, const std::string& dest);
        void generateNumber(const NodeNumber* number, const std::string& dest);
        void generateIdentifier(const NodeIdentifier* ident, const std::string& dest);
        void generateCall(const NodeCall* call, const std::string& dest);
        void generateFunctionExitInline(const NodeCall* call);

        /*
        void generateFunction(const NodeFunction& function);
        
        void generateStatement(const NodeStatement& statement);
        void generateStatementExit(NodeStatementExit* statement);
        void generateStatementLet(NodeStatementLet* statement);
        void generateStatementFunctionCall(const NodeStatementFunctionCall* statement);
        void generateStatementRet(const NodeStatementRet* statement);

        void generateExpression(NodeExpression* expression, const std::string& destinationRegister);
        void generateAtom(const NodeAtom* expression, const std::string& destinationRegister);
        void generateExpressionIntLit(const NodeExpressionIntLit* expression, const std::string& destinationRegister);
        void generateExpressionIdentifier(const NodeExpressionIdentifier* expression, const std::string& destinationRegister);
        void generateExpressionAdd(NodeExpressionAdd* expression, const std::string& destinationRegister, const std::string& tempRegister);
        */
    };

}
