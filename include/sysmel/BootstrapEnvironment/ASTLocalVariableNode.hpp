#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_LOCAL_VARIABLE_NODE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_LOCAL_VARIABLE_NODE_HPP
#pragma once

#include "ASTVariableNode.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am a node that is used for declaring or defining a program entity.
 */
class ASTLocalVariableNode : public SubtypeOf<ASTVariableNode, ASTLocalVariableNode>
{
public:
    static constexpr char const __typeName__[] = "ASTLocalVariableNode";

    virtual bool isASTLocalVariableNode() const override;
    virtual AnyValuePtr accept(const ASTVisitorPtr &visitor) override;
    virtual SExpression asSExpression() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_LOCAL_VARIABLE_NODE_HPP