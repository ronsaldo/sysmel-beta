#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_MAKE_ASSOCIATION_NODE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_MAKE_ASSOCIATION_NODE_HPP
#pragma once

#include "ASTNode.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am an AST node whose job is to make a tuple.
 */
class ASTMakeAssociationNode : public SubtypeOf<ASTNode, ASTMakeAssociationNode>
{
public:
    static constexpr char const __typeName__[] = "ASTMakeAssociationNode";

    virtual bool isASTMakeAssociationNode() const override;
    virtual SExpression asSExpression() const override;

    ASTNodePtr key;
    ASTNodePtr value;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_MAKE_ASSOCIATION_NODE_HPP