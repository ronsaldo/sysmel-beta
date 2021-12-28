#ifndef SYSMEL_ENVIRONMENT_SPECIFIC_METHOD_HPP
#define SYSMEL_ENVIRONMENT_SPECIFIC_METHOD_HPP
#pragma once

#include "Method.hpp"
#include "MethodFlags.hpp"

namespace Sysmel
{
namespace Environment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(FunctionalTypeValue)

/**
 * I am the base interface for most of the methods that are defined in the system.
 */
class SYSMEL_COMPILER_LIB_EXPORT SpecificMethod : public SubtypeOf<Method, SpecificMethod>
{
public:
    static constexpr char const __typeName__[] = "SpecificMethod";

    virtual bool isSpecificMethod() const override;

    const FunctionalTypePtr &getFunctionalType() const
    {
        return functionalType;
    }

    virtual void setMethodSignature(const TypePtr &receiverType, const TypePtr &resultType, const TypePtrList &argumentTypes);
    virtual void setFunctionSignature(const TypePtr &resultType, const TypePtrList &argumentTypes);
    virtual void setClosureSignature(const TypePtr &resultType, const TypePtrList &argumentTypes);

    virtual TypePtr getExpectedTypeForAnalyzingArgumentWithIndex(size_t argumentIndex) override;
    virtual MethodPatternMatchingResult matchPatternForRunWithIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver) override;
    virtual MethodPatternMatchingResult matchPatternForAnalyzingMessageSendNode(const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer) override;
    virtual AnyValuePtr asMethodMatchingSignature(const TypePtr &receiverType, const TypePtrList &argumentTypes, const TypePtr &resultType) override;

    virtual bool isMacroMethod() const override;

    virtual ASTNodePtr analyzeMessageSendNode(const ASTMessageSendNodePtr &node, const ASTSemanticAnalyzerPtr &semanticAnalyzer) override;
    virtual ASTNodePtr analyzeIdentifierReferenceNode(const ASTIdentifierReferenceNodePtr &partiallyAnalyzedNode, const ASTSemanticAnalyzerPtr &semanticAnalyzer) override;

    virtual SExpression asSExpression() const override;

    virtual FunctionalTypeValuePtr asFunctionalValue();

    void setIntrinsicName(const AnyValuePtr &theIntrinsicName);
    const AnyValuePtr &getIntrinsicName() const;

    void concretizeAutoResultTypeWith(const TypePtr &newResultType);

    void addMethodFlags(MethodFlags extraMethodFlags);
    MethodFlags getMethodFlags() const;
    void setMethodFlags(MethodFlags newFlags);

    bool isConstructor() const;
    void makeConstructor();

    bool isExplicit() const;
    void makeExplicit();

    bool isPure() const;
    void makePure();

    bool isConversion() const;
    void makeConversion();

    bool isTrivial() const;
    void makeTrivial();

protected:
    FunctionalTypePtr functionalType;
    FunctionalTypeValuePtr functionalValue;
    AnyValuePtr intrinsicName;

    MethodFlags methodFlags = MethodFlags::None;
};

} // End of namespace Environment
} // End of namespace Sysmel

#endif //SYSMEL_ENVIRONMENT_SPECIFIC_METHOD_HPP