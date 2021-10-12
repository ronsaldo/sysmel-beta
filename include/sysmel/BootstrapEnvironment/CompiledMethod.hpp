#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_COMPILED_METHOD_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_COMPILED_METHOD_HPP
#pragma once

#include "SpecificMethod.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(ASTSourcePosition);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(ASTAnalysisEnvironment);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(CompiledMethod);
SYSMEL_DECLARE_BOOTSTRAP_CLASS_AND_LIST(ArgumentVariable);
SYSMEL_DECLARE_BOOTSTRAP_CLASS_AND_LIST(FunctionalType);

/**
 * I am the base interface for most of the methods that are defined in the system.
 */
class CompiledMethod : public SubtypeOf<SpecificMethod, CompiledMethod>
{
public:
    static constexpr char const __typeName__[] = "CompiledMethod";

    virtual bool isCompiledMethod() const override;

    bool isDefined() const;
    bool isDefinedForCompileTime() const;

    void setDeclaration(const ASTNodePtr &node);
    void setFunctionalType(const FunctionalTypePtr &type);
    void setDefinition(const ASTNodePtr &node, const ASTNodePtr &bodyNode, const ASTAnalysisEnvironmentPtr &environment);

    virtual void ensureSemanticAnalysis() override;

    virtual std::string fullPrintString() const override;
    virtual SExpression asFullDefinitionSExpression() const override;

    virtual AnyValuePtr runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver) override;

    virtual void recordChildProgramEntityDefinition(const ProgramEntityPtr &newChild) override;

    virtual const FunctionalTypePtr &getFunctionalType() const;

protected:
    ASTAnalysisEnvironmentPtr createSemanticAnalysisEnvironment();

    ASTSourcePositionPtr declarationPosition;
    ASTNodePtr declarationNode;

    ASTSourcePositionPtr definitionPosition;
    ASTNodePtr definitionNode;
    ASTNodePtr definitionBodyNode;

    ASTAnalysisEnvironmentPtr definitionEnvironment;
    ASTNodePtr analyzedBodyNode;

    ProgramEntityPtrList children;

    ArgumentVariablePtrList arguments;
    FunctionalTypePtr functionalType;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_COMPILED_METHOD_HPP