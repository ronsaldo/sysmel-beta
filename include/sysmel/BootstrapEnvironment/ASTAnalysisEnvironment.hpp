#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_ANALYSIS_ENVIRONMENT_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_ANALYSIS_ENVIRONMENT_HPP
#pragma once

#include "CompilerObject.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(ASTAnalysisEnvironment)
SYSMEL_DECLARE_BOOTSTRAP_CLASS(LexicalScope)
SYSMEL_DECLARE_BOOTSTRAP_CLASS(CleanUpScope)
SYSMEL_DECLARE_BOOTSTRAP_CLASS(LanguageSupport);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(ProgramEntity);

/**
 * I specify an AST analysis environment.
 */
class ASTAnalysisEnvironment : public SubtypeOf<CompilerObject, ASTAnalysisEnvironment>
{
public:
    static constexpr char const __typeName__[] = "ASTAnalysisEnvironment";

    ASTAnalysisEnvironmentPtr copyWithLexicalScope(const LexicalScopePtr &newScope);
    ASTAnalysisEnvironmentPtr copyWithCleanUpcope(const CleanUpScopePtr &cleanUpScope);
    ASTAnalysisEnvironmentPtr copyForPublicProgramEntityBody(const ProgramEntityPtr &publicProgramEntity);

    LexicalScopePtr lexicalScope;
    CleanUpScopePtr cleanUpScope;
    LanguageSupportPtr languageSupport;

    ProgramEntityPtr programEntityForPublicDefinitions;
    ProgramEntityPtr localDefinitionsOwner;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_AST_ANALYSIS_ENVIRONMENT_HPP