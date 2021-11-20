#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_VALUE_AS_VOID_TYPE_CONVERSION_RULE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_VALUE_AS_VOID_TYPE_CONVERSION_RULE_HPP
#pragma once

#include "TypeConversionRule.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am an instance of a function type object.
 */
class ValueAsVoidTypeConversionRule : public SubtypeOf<TypeConversionRule, ValueAsVoidTypeConversionRule>
{
public:
    static constexpr char const __typeName__[] = "ValueAsVoidTypeConversionRule";

    static TypeConversionRulePtr uniqueInstance();

    virtual bool canBeUsedToConvertNodeFromTo(const ASTNodePtr &node, const TypePtr &sourceType, const TypePtr &targetType) const override;
    virtual size_t getConversionCost(const ASTNodePtr &node, const TypePtr &targetType) const override;
    virtual ASTNodePtr convertNodeIntoWith(const ASTNodePtr &node, const TypePtr &targetType, const ASTSemanticAnalyzerPtr &semanticAnalyzer) const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_VALUE_AS_VOID_TYPE_CONVERSION_RULE_HPP