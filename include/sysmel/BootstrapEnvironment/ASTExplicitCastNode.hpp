#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EXPLICIT_CAST_NODE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EXPLICIT_CAST_NODE_HPP
#pragma once

#include "ASTCastNode.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am a node that is used for accessing a variable.
 */
class ASTExplicitCastNode : public SubtypeOf<ASTCastNode, ASTExplicitCastNode>
{
public:
    static constexpr char const __typeName__[] = "ASTExplicitCastNode";

    virtual bool isASTExplicitCastNode() const override;
    virtual AnyValuePtr accept(const ASTVisitorPtr &visitor) override;
    virtual SExpression asSExpression() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EXPLICIT_CAST_NODE_HPP