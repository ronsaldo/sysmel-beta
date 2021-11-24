#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_CONTINUE_NODE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_CONTINUE_NODE_HPP
#pragma once

#include "ASTNode.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am a node that is used for declaring or defining a type program entity.
 */
class ASTContinueNode : public SubtypeOf<ASTNode, ASTContinueNode>
{
public:
    static constexpr char const __typeName__[] = "ASTContinueNode";

    virtual bool isASTContinueNode() const override;
    virtual AnyValuePtr accept(const ASTVisitorPtr &visitor) override;
    virtual SExpression asSExpression() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_CONTINUE_NODE_HPP