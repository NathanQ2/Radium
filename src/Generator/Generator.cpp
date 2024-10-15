#include "Generator.h"

#include <utility>
#include <iostream>

#include "../Logger/Logger.h"


namespace Radium
{
    Generator::Generator(NodeRoot nodeRoot)
        : m_nodeRoot(std::move(nodeRoot)),
          m_stackSizeBytes(0)
    {
        m_registers["rax"] = false;
        m_registers["rbx"] = false;
        m_registers["rcx"] = false;
        m_registers["rdx"] = false;
        m_registers["rsi"] = false;
        m_registers["rdi"] = false;
        m_registers["rsp"] = false;
        m_registers["rbp"] = false;
    }

    std::string Generator::generate()
    {
        // boilerplate
        m_sstream << "global _start\n\n_start:\n";
        for (const NodeStatement& statement : m_nodeRoot.statements)
        {
            generateStatement(statement);
            m_sstream << "\n";
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

    std::string Generator::pushRegister()
    {
        for(const auto&[reg, used] : m_registers)
        {
            if (used == false) {
                m_registers[reg] = true;
                return reg;
            }
        }

        RA_ERROR("Ran out of registers!");
        exit(EXIT_FAILURE);
    }

    void Generator::popRegister(const std::string& reg)
    {
        m_registers[reg] = false;
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
        m_sstream << "    ; exit\n";

        generateExpression(statement.expression, "rdi");
        mov("rax", "60");
        m_sstream << "    syscall\n";
    }

    void Generator::generateStatementLet(const NodeStatementLet &statement)
    {
        const std::string reg = pushRegister();
        m_sstream << "    ; let\n";
        generateExpression(statement.expression, reg);
        if(m_identifierStackPositions.contains(statement.identifier))
        {
            RA_ERROR("Identifier '{0}' has already been declared!", statement.identifier);

            exit(EXIT_FAILURE);
        }
        push(reg, 8);
        popRegister(reg);
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
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIdentifier>)
            {
                generateExpressionIdentifier(expr, desinationRegister);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionAdd>)
            {
                const std::string reg = pushRegister();
                generateExpressionAdd(expr, desinationRegister, reg);
                popRegister(reg);
            }
        }, expression.variant);
    }

    void Generator::generateExpressionIntLit(const NodeExpressionIntLit &expression,
        const std::string &destinationRegister)
    {
        mov(destinationRegister, expression.value);
    }

    void Generator::generateExpressionIdentifier(const NodeExpressionIdentifier &expression,
        const std::string &destinationRegister)
    {
        if (!m_identifierStackPositions.contains(expression.value))
        {
            RA_ERROR("Identifier '{0}' has not been declared!");

            exit(EXIT_FAILURE);
        }
        const size_t offset = m_identifierStackPositions[expression.value];
        // Copy value onto top of the stack
        std::stringstream ss;
        ss << "QWORD [rsp+" << (m_stackSizeBytes - offset) << "]";
        push(ss.str(), 8);
        pop(destinationRegister, 8);
    }

    void Generator::generateExpressionAdd(const NodeExpressionAdd &expression, const std::string &destinationRegister,
        const std::string &tempRegister)
    {
        generateExpression(expression.lhs, destinationRegister);
        generateExpression(expression.rhs, tempRegister);

        m_sstream << "    add " << destinationRegister << ", " << tempRegister << "\n";
    }
}
