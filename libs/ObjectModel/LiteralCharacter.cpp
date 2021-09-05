#include "sysmel/ObjectModel/LiteralCharacter.hpp"
#include "sysmel/ObjectModel/StringUtilities.hpp"
#include <algorithm>

namespace SysmelMoebius
{
namespace ObjectModel
{

bool LiteralCharacter::isLiteralCharacter() const
{
    return true;
}

std::string LiteralCharacter::asString() const
{
    std::string result;
    result.reserve(1);
    result[0] = value.wordAt(0);
    return result;
}

std::string LiteralCharacter::printString() const
{
    std::ostringstream out;
    out << '\'';
    formatUtf32Character(value.wordAt(0), out);
    out << '\'';
    return out.str();
}

} // End of namespace ObjectModel
} // End of namespace SysmelMoebius