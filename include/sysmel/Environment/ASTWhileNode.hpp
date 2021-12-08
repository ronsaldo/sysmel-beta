#ifndef SYSMEL_ENVIRONMENT_AST_WHILE_NODE_HPP
#define SYSMEL_ENVIRONMENT_AST_WHILE_NODE_HPP
#pragma once

#include "ASTNode.hpp"

namespace Sysmel
{
namespace Environment
{

/**
 * I am a node that is used for declaring or defining a type program entity.
 */
class ASTWhileNode : public SubtypeOf<ASTNode, ASTWhileNode>
{
public:
    static constexpr char const __typeName__[] = "ASTWhileNode";

    virtual bool isASTWhileNode() const override;
    virtual AnyValuePtr accept(const ASTVisitorPtr &visitor) override;
    virtual SExpression asSExpression() const override;

    virtual void childrenDo(const ASTIterationBlock &aBlock) override;

    ASTNodePtr condition;
    ASTNodePtr bodyExpression;
    ASTNodePtr continueExpression;
};

} // End of namespace Environment
} // End of namespace Sysmel

#endif //SYSMEL_ENVIRONMENT_AST_WHILE_NODE_HPP