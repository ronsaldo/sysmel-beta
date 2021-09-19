#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EMPTY_SOURCE_POSITION_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EMPTY_SOURCE_POSITION_HPP
#pragma once

#include "ASTSourcePosition.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am the interface for all of the language independent AST nodes.
 */
class ASTEmptySourcePosition : public SubtypeOf<ASTSourcePosition, ASTEmptySourcePosition>
{
public:
    static constexpr char const __typeName__[] = "ASTEmptySourcePosition";
    
    virtual bool isASTEmptySourcePosition() const override;
    virtual SExpression asSExpression() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_EMPTY_SOURCE_POSITION_HPP