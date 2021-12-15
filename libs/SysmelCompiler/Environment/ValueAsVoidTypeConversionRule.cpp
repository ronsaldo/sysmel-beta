#include "Environment/ValueAsVoidTypeConversionRule.hpp"
#include "Environment/ASTSemanticAnalyzer.hpp"
#include "Environment/ASTValueAsVoidTypeConversionNode.hpp"
#include "Environment/Type.hpp"
#include "Environment/BootstrapTypeRegistration.hpp"
#include "Environment/StringUtilities.hpp"

namespace Sysmel
{
namespace Environment
{
static BootstrapTypeRegistration<ValueAsVoidTypeConversionRule> valueAsVoidTypeConversionRuleRegistration;

TypeConversionRulePtr ValueAsVoidTypeConversionRule::uniqueInstance()
{
    static TypeConversionRulePtr singleton = basicMakeGlobalSingletonObject<ValueAsVoidTypeConversionRule> ();
    return singleton;
}

bool ValueAsVoidTypeConversionRule::canBeUsedToConvertNodeFromTo(const ASTNodePtr &node, const TypePtr &sourceType, const TypePtr &targetType) const
{
    (void)node;
    return !sourceType->isVoidType() && targetType->isVoidType();
}

size_t ValueAsVoidTypeConversionRule::getConversionCost(const ASTNodePtr &node, const TypePtr &targetType) const
{
    (void)node;
    (void)targetType;
    return 0;
}

ASTNodePtr ValueAsVoidTypeConversionRule::convertNodeAtIntoWith(const ASTNodePtr &node, const ASTSourcePositionPtr &sourcePosition, const TypePtr &targetType, const ASTSemanticAnalyzerPtr &semanticAnalyzer) const
{
    (void)semanticAnalyzer;
    assert(node->analyzedType);

    auto result = basicMakeObject<ASTValueAsVoidTypeConversionNode> ();
    result->sourcePosition = sourcePosition;
    result->expression = node;
    result->analyzedType = targetType;
    return result;
}

} // End of namespace Environment
} // End of namespace Sysmel