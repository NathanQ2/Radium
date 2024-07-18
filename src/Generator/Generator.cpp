#include "Generator.h"

#include <utility>
#include <iostream>


namespace Radium::Generator
{
    Generator::Generator(NodeRoot nodeRoot)
        : m_nodeRoot(std::move(nodeRoot)),
          m_sstream(),
          m_stackSizeBytes(0)
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

    void Generator::mov(const std::string &reg, const std::string &val)
    {
        m_sstream << "    mov " << reg << ", " << val << "\n";
    }

    void Generator::push(const std::string &reg, const size_t sizeBytes)
    {
        m_sstream << "    push " << reg << "\n";
        m_stackSizeBytes += sizeBytes;
    }

    void Generator::pop(const std::string &reg, const size_t sizeBytes)
    {
        m_sstream << "    pop " << reg << "\n";
        m_stackSizeBytes -= sizeBytes;
    }

    void Generator::generateStatement(const NodeStatement& statement)
    {
        std::visit([this]<typename T>(T&& stmt)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeStatementExit>)
            {
                generateStatementExit(stmt);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeStatementLet>)
            {
                generateStatementLet(stmt);
            }
        }, statement.variant);
    }

    void Generator::generateStatementExit(const NodeStatementExit& statement)
    {
        mov("rax", "60");
        generateExpression(statement.expression, "rdi");
        m_sstream << "    syscall\n";
    }

    void Generator::generateStatementLet(const NodeStatementLet &statement)
    {
        generateExpression(statement.expression, "rax");
        if(m_identifierStackPositions.contains(statement.identifier))
        {
            std::cerr << "ERROR: Identifier `" << statement.identifier << "` Has been declared twice!" << std::endl;

            exit(EXIT_FAILURE);
        }
        push("rax", 8);
        m_identifierStackPositions[statement.identifier] = m_stackSizeBytes;
    }

    void Generator::generateExpression(const NodeExpression &expression, const std::string &desinationRegister)
    {
        std::visit([this, desinationRegister]<typename T>(T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIntLit>)
            {
                generateExpressionIntLit(expr, desinationRegister);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIdentifer>)
            {
                generateExpressionIdentifier(expr, desinationRegister);
            }
        }, expression.variant);
    }

    void Generator::generateExpressionIntLit(const NodeExpressionIntLit &expression,
        const std::string &destinationRegister)
    {
        mov(destinationRegister, expression.value);
    }

    void Generator::generateExpressionIdentifier(const NodeExpressionIdentifer &expression,
        const std::string &destinationRegister)
    {
        if (!m_identifierStackPositions.contains(expression.value))
        {
            std::cerr << "ERROR: identifier `" << expression.value << "` has not been defined!" << std::endl;

            exit(EXIT_FAILURE);
        }
        const size_t offset = m_identifierStackPositions[expression.value];
        // Copy value onto top of the stack
        std::stringstream ss;
        ss << "QWORD [rsp+" << (m_stackSizeBytes - offset) << "]";
        push(ss.str(), 8);
        pop(destinationRegister, 8);
    }
}
