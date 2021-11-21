#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_VALUE_UPCAST_TYPE_CONVERSION_NODE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_VALUE_UPCAST_TYPE_CONVERSION_NODE_HPP
#pragma once

#include "ASTTypeConversionNode.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am a node that is used for declaring or defining a type program entity.
 */
class ASTUpcastTypeConversionNode : public SubtypeOf<ASTTypeConversionNode, ASTUpcastTypeConversionNode>
{
public:
    static constexpr char const __typeName__[] = "ASTUpcastTypeConversionNode";

    virtual bool isASTUpcastTypeConversionNode() const override;
    virtual bool isPureCompileTimeLiteralValueNode() const override;
    
    virtual AnyValuePtr accept(const ASTVisitorPtr &visitor) override;
    virtual SExpression asSExpression() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_VALUE_UPCAST_TYPE_CONVERSION_NODE_HPP