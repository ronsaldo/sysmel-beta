#include "sysmel/BootstrapEnvironment/IdentifierLookupScope.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<IdentifierLookupScope> IdentifierLookupScopeRegistration;

bool IdentifierLookupScope::isIdentifierLookupScope() const
{
    return true;
}

bool IdentifierLookupScope::isLexicalScope() const
{
    return false;
}

AnyValuePtr IdentifierLookupScope::lookupSymbolLocally(const AnyValuePtr &)
{
    return nullptr;
}

AnyValuePtr IdentifierLookupScope::lookupSymbolRecursively(const AnyValuePtr &symbol)
{
    auto found = lookupSymbolLocally(symbol);
    if(found)
        return found;
    return parent ? parent->lookupSymbolRecursively(symbol) : nullptr;
}

bool IdentifierLookupScope::isNameReservedLocally(const AnyValuePtr &)
{
    return false;
}

bool IdentifierLookupScope::isNameReserved(const AnyValuePtr &symbol)
{
    if(isNameReservedLocally(symbol))
        return true;

    return parent ? parent->isNameReserved(symbol) : false;
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius