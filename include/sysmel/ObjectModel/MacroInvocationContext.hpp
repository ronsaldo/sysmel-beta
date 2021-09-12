#ifndef SYSMEL_COMPILER_OBJECT_MODEL_MACRO_INVOCATION_CONTEXT_HPP
#define SYSMEL_COMPILER_OBJECT_MODEL_MACRO_INVOCATION_CONTEXT_HPP
#pragma once

#include "CompilerObject.hpp"

namespace SysmelMoebius
{
namespace ObjectModel
{

/**
 * I am the base interface for compile time object that is passed through the interpreter.
 */
class MacroInvocationContext : public SubtypeOf<CompilerObject, MacroInvocationContext>
{
public:
    static constexpr char const __typeName__[] = "MacroInvocationContext";

    virtual bool isMacroInvocationContext() const override;

    ASTNodePtr receiverNode;
    TypePtr selfType;
    ASTBuilderPtr astBuilder;
};

} // End of namespace ObjectModel
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_OBJECT_MODEL_LITERAL_VALUE_HPP