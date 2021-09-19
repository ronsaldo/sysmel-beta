#include "sysmel/BootstrapEnvironment/LiteralSymbol.hpp"
#include "sysmel/BootstrapEnvironment/StringUtilities.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"
#include <unordered_map>
#include <sstream>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{
static BootstrapTypeRegistration<LiteralSymbol> literalSymbolTypeRegistration;

static std::unordered_map<std::string, std::shared_ptr<LiteralSymbol>> SymbolInternTable;

std::shared_ptr<LiteralSymbol> LiteralSymbol::intern(const std::string &value)
{
    auto it = SymbolInternTable.find(value);
    if(it != SymbolInternTable.end())
    {
        return it->second;
    }

    auto newSymbol = std::make_shared<LiteralSymbol> ();
    newSymbol->value = value;
    SymbolInternTable.insert(std::make_pair(value, newSymbol));
    return newSymbol;
}

AnyValuePtr internSymbol(const std::string &symbolValue)
{
    return LiteralSymbol::intern(symbolValue);
}

LiteralSymbolPtr LiteralSymbol::makeFor(const std::string &value)
{
    return intern(value);
}

bool LiteralSymbol::isLiteralSymbol() const
{
    return true;
}

std::string LiteralSymbol::printString() const
{
    std::ostringstream out;
    out << "#\"";
    for(auto c : value)
    {
        formatUtf8Character(c, out);
    }
    out << '"';
    return out.str();
}

SExpression LiteralSymbol::asSExpression() const
{
    return SExpressionSymbol{value};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius