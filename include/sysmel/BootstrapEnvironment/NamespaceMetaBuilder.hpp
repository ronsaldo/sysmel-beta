#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_NAMESPACE_METABUILDER_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_NAMESPACE_METABUILDER_HPP
#pragma once

#include "NamedProgramEntityMetaBuilder.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(NamespaceMetaBuilder);

/**
 * I am the base interface for a meta-builder.
 */
class NamespaceMetaBuilder : public SubtypeOf<NamedProgramEntityMetaBuilder, NamespaceMetaBuilder>
{
public:
    static constexpr char const __typeName__[] = "NamespaceMetaBuilder";

protected:
    virtual ASTNodePtr concretizeMetaBuilder() override;
    virtual ASTNodePtr analyzeMessageSendNodeWithSelector(const std::string &selectorValue, const ASTMessageSendNodePtr &partiallyAnalyzedNode, const ASTSemanticAnalyzerPtr &semanticAnalyzer) override;

    ASTNodePtr bodyNode;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_NAMESPACE_METABUILDER_HPP
