#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_LEXICAL_SCOPE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_LEXICAL_SCOPE_HPP
#pragma once

#include "IdentifierLookupScope.hpp"
#include <unordered_map>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(LexicalScope);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(ProgramEntity);

/**
 * I am the interface for the scope that is used for the identifier lookup.
 */
class LexicalScope : public SubtypeOf<IdentifierLookupScope, LexicalScope>
{
public:
    static constexpr char const __typeName__[] = "LexicalScope";

    static LexicalScopePtr makeEmpty();
    static LexicalScopePtr makeWithParent(const IdentifierLookupScopePtr &parent);

    virtual bool isLexicalScope() const override;

    virtual AnyValuePtr lookupSymbolLocally(const AnyValuePtr &symbol) override;
    virtual AnyValuePtr lookupSymbolRecursively(const AnyValuePtr &symbol) override;

    virtual void setSymbolBinding(const AnyValuePtr &symbol, const AnyValuePtr &binding);

    void lockForNewDefinitions();

    void useNamespace(const ProgramEntityPtr &namespaceProgramEntity);
private:
    bool lockedForNewDefinitions;
    std::unordered_map<AnyValuePtr, AnyValuePtr> boundSymbols;
    std::vector<ProgramEntityPtr> usedNamespaces;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_IDENTIFIER_LOOKUP_SCOPE_HPP