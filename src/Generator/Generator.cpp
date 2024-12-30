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
        m_ss << "global _start\n\n_start:\n";
        for (const NodeStatement& statement : m_nodeRoot.statements)
        {
            generateStatement(statement);
            m_ss << "\n";
        }

        return m_ss.str();
    }

    void Generator::mov(const std::string &dest, const std::string &val)
    {
        m_ss << "    mov " << dest << ", " << val << "\n";
    }

    void Generator::push(const std::string &reg, const size_t sizeBytes)
    {
        m_ss << "    push " << reg << "\n";
        m_stackSizeBytes += sizeBytes;
    }

    void Generator::pop(const std::string &reg, const size_t sizeBytes)
    {
        m_ss << "    pop " << reg << "\n";
        m_stackSizeBytes -= sizeBytes;
    }

    std::string Generator::reserveRegister()
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

    void Generator::freeRegister(const std::string& reg)
    {
        m_registers[reg] = false;
    }

    void Generator::generateStatement(const NodeStatement& statement)
    {
        std::visit([this]<typename T>(T&& stmt)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeStatementExit*>)
            {
                generateStatementExit(stmt);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeStatementLet*>)
            {
                generateStatementLet(stmt);
            }
        }, statement.variant);
    }

    void Generator::generateStatementExit(NodeStatementExit* statement)
    {
        m_ss << "    ; exit\n";

        std::string reg = reserveRegister();
        generateExpression(statement->exitCode, reg);
        mov("rdi", reg);
        freeRegister(reg);
        mov("rax", "60");
        m_ss << "    syscall\n";
    }

    void Generator::generateStatementLet(NodeStatementLet* statement)
    {
        const std::string reg = reserveRegister();
        m_ss << "    ; let\n";
        NodeExpression* expr = statement->value;
        // is moving the right operation here?
        generateExpression(expr, reg);
        if(m_identifierStackPositions.contains(statement->identifier))
        {
            RA_ERROR("Identifier '{0}' has already been declared!", statement->identifier);

            exit(EXIT_FAILURE);
        }
        
        push(reg, 8);
        freeRegister(reg);
        m_identifierStackPositions[statement->identifier] = m_stackSizeBytes;
    }

    void Generator::generateExpression(NodeExpression* expression, const std::string &destinationRegister)
    {
        std::visit([this, destinationRegister]<typename T>(T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeAtom*>)
            {
                generateAtom(expr, destinationRegister);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionAdd*>)
            {
                const std::string reg = reserveRegister();
                generateExpressionAdd(expr, destinationRegister, reg);
                freeRegister(reg);
            }
        }, expression->variant);
    }

    void Generator::generateAtom(const NodeAtom* expression, const std::string& destinationRegister)
    {
        std::visit([this, destinationRegister]<typename T>(T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIntLit*>)
            {
                generateExpressionIntLit(expr, destinationRegister);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionIdentifier*>)
            {
                generateExpressionIdentifier(expr, destinationRegister);
            }
        }, expression->variant);
    }

    void Generator::generateExpressionIntLit(const NodeExpressionIntLit* expression,
                                             const std::string &destinationRegister)
    {
        mov(destinationRegister, expression->value);
    }

    void Generator::generateExpressionIdentifier(const NodeExpressionIdentifier* expression,
        const std::string &destinationRegister)
    {
        if (!m_identifierStackPositions.contains(expression->value))
        {
            RA_ERROR("Identifier '{0}' has not been declared!", expression->value);

            exit(EXIT_FAILURE);
        }
        
        const size_t offset = m_identifierStackPositions[expression->value];
        // Copy value onto top of the stack
        std::stringstream ss;
        ss << "QWORD [rsp+" << (m_stackSizeBytes - offset) << "]";
        push(ss.str(), 8);
        pop(destinationRegister, 8);
    }

    void Generator::generateExpressionAdd(NodeExpressionAdd* expression, const std::string &destinationRegister,
        const std::string &tempRegister)
    {
        generateExpression(expression->lhs, destinationRegister);
        generateExpression(expression->rhs, tempRegister);

        m_ss << "    add " << destinationRegister << ", " << tempRegister << "\n";
    }
}
