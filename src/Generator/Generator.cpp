#include "Generator.h"

#include <utility>


namespace Radium::Generator
{
    Generator::Generator(NodeRoot nodeRoot)
        : m_nodeRoot(std::move(nodeRoot)),
        m_sstream()
    {
    }

    std::string Generator::generate()
    {
        // boilerplate
        m_sstream << "global _start\n_start:\n";
        for (NodeStatement& statement : m_nodeRoot.statements)
        {
            generateStatement(statement);
        }

        return m_sstream.str();
    }

    void Generator::generateStatement(const NodeStatement& statement)
    {
        std::visit([this]<typename T>(T&& stmt)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeStatementExit>)
            {
                generateStatementExit(stmt);
            }
        }, statement.variant);
    }

    void Generator::generateStatementExit(const NodeStatementExit& statement)
    {
        m_sstream << "    mov rax, 60\n";
        generateExpression(statement.expression, "rdi");
        m_sstream << "    syscall\n";
    }

    void Generator::generateExpression(const NodeExpression &expression, const std::string &desinationRegister)
    {
        std::visit([this, desinationRegister]<typename T>(T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIntLit>)
            {
                generateExpressionIntLit(expr, desinationRegister);
            }
        }, expression.variant);
    }

    void Generator::generateExpressionIntLit(const NodeExpressionIntLit &expression,
        const std::string &destinationRegister)
    {
        m_sstream << "    mov " << destinationRegister << ", " << expression.value << "\n";
    }
}
