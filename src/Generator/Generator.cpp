#include "Generator.h"

#include <utility>
#include <iostream>

#include "../Logger/Logger.h"


namespace Radium
{
    Generator::Generator(const CompilationUnit& cu)
        : m_cu(cu),
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
        m_ss << "global _start\n\n";

        /*
        for (const NodeFunction* function : m_nodeRoot.functions)
        {
            if (function->identifier == "main") generateFunction(NodeFunction(function->statements, "_start"));
            else generateFunction(*function);
        }
        */
        
        const std::vector<Module> modules = m_cu.getModules();
        for (const Module& module : modules)
        {
            if (m_generatedModules[module.getPath()] == false)
                generateModule(module);
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

    void Generator::generateModule(const Module& module)
    {
        const NodeRoot& root = module.getRoot();
        
        for (const NodeModuleInclude* include : root.includes)
        {
            generateModule(m_cu.getModuleByPath(include->modPath));
        }
        
        for (const NodeFunctionDecl* func : root.functions)
        {
            generateFunction(func);
        }
        
        m_generatedModules[module.getPath()] = true;
    }

    void Generator::generateFunction(const NodeFunctionDecl* func)
    {
        std::string funcName = func->identifier->value;
        if (funcName == "main") funcName = "_start";
        
        m_ss << funcName << ":\n";
        generateBlock(func->block);
    }

    void Generator::generateBlock(const NodeBlock* block)
    {
        for (const NodeStatement* statement : block->statements)
        {
            generateStatement(statement);
        }
    }

    void Generator::generateStatement(const NodeStatement* statement)
    {
        std::visit([this]<typename T>(T&& stmt)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeVarDecl*>) generateVarDecl(stmt);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeExpressionStatement*>) generateExpressionStatement(stmt);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeReturnStatement*>) generateReturnStatement(stmt);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeBlock*>) generateBlock(stmt);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeIfStatement*>) generateIfStatement(stmt);
        }, statement->stmt);

        m_ss << "\n";
    }

    void Generator::generateVarDecl(const NodeVarDecl* varDecl)
    {
        m_ss << "    ; let\n";
        const std::string dest = reserveRegister();
        generateAssignment(varDecl->assignment, dest);
    }

    void Generator::generateExpressionStatement(const NodeExpressionStatement* statement)
    {
        std::string dest = reserveRegister();
        generateExpression(statement->expression, dest);
        freeRegister(dest);
    }

    void Generator::generateReturnStatement(const NodeReturnStatement* ret)
    {
        m_ss << "    ; ret\n";
        std::string dest = reserveRegister();
        generateExpression(ret->expression, dest);
        mov("rax", dest);
        m_ss << "    ret\n";
    }

    void Generator::generateIfStatement(const NodeIfStatement* ifStmt)
    {
        std::string reg = reserveRegister();
        m_ss << "    ; if\n";
        generateExpression(ifStmt->expr, reg);

        std::string label = nextLabel();

        m_ss << "    cmp " << reg << ", 0\n";
        m_ss << "    jne " << label << "\n";
        m_ss << "    ; begin true\n";
        generateBlock(ifStmt->block);
        m_ss << "    ; end true\n";
        m_ss << label << ": \n";
    }

    void Generator::generateExpression(const NodeExpression* expr, const std::string& dest)
    {
        std::visit([this, dest]<typename T> (T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeAssignment*>) generateAssignment(expr, dest);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeAdditive*>) generateAdditive(expr, dest);
        }, expr->expr);
    }

    void Generator::generateAssignment(const NodeAssignment* assignment, const std::string& dest)
    {
        std::visit([this, dest]<typename T> (T&& expr)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, std::pair<NodeIdentifier*, NodeExpression*>>) generateAssignment(expr, dest);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeAdditive*>) generateAdditive(expr, dest);
        }, assignment->assignment);
    }

    void Generator::generateAssignment(std::pair<NodeIdentifier*, NodeExpression*> assignment, const std::string& dest)
    {
        generateExpression(assignment.second, dest);
        push(dest, 8);
        m_identifierStackPositions[assignment.first->value] = m_stackSizeBytes;
    }

    void Generator::generateAdditive(const NodeAdditive* additive, const std::string& dest)
    {
        generateMultiplicative(additive->left, dest);
        if (additive->right != nullptr)
        {
            std::string temp = reserveRegister();
            generateMultiplicative(additive->right, temp);
            if (additive->isSubtraction) m_ss << "    sub ";
            else                         m_ss << "    add ";
            m_ss << dest << ", " << temp << "\n";
            freeRegister(temp);
        }
    }

    void Generator::generateMultiplicative(const NodeMultiplicative* multi, const std::string& dest)
    {
        generatePrimary(multi->primary, dest);
    }

    void Generator::generatePrimary(const NodePrimary* primary, const std::string& dest)
    {
        std::visit([this, dest]<typename T> (T&& val)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, NodeNumber*>) generateNumber(val, dest);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeIdentifier*>) generateIdentifier(val, dest);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeCall*>) generateCall(val, dest);
            if constexpr (std::is_same_v<std::decay_t<T>, NodeExpression*>) generateExpression(val, dest);
        }, primary->value);
    }

    void Generator::generateNumber(const NodeNumber* number, const std::string& dest)
    {
        mov(dest, number->value);
    }

    void Generator::generateIdentifier(const NodeIdentifier* ident, const std::string& dest)
    {
        const size_t offset = m_identifierStackPositions[ident->value];
        std::stringstream ss;
        ss << "QWORD [rsp+" << (m_stackSizeBytes - offset) << "]";
        push(ss.str(), 8);
        pop(dest, 8);
    }

    void Generator::generateCall(const NodeCall* call, const std::string& dest)
    {
        if (call->identifier->value == "exit")
        {
            generateFunctionExitInline(call);
        }
        else
        {
            m_ss << "    ; call " << call->identifier->value << "\n";
            m_ss << "    call " << call->identifier->value << "\n";
            mov(dest, "rax");
        }
    }

    void Generator::generateFunctionExitInline(const NodeCall* call)
    {
        if (call->argList->args.size() != 1)
        {
            RA_ERROR("Invalid number of args for 'exit' Expected 1");
            exit(EXIT_FAILURE);
        }
        
        m_ss << "    ; exit\n";
        mov("rax", "60");
        generateExpression(call->argList->args[0], "rdi");
        m_ss << "    syscall\n";
    }
}
