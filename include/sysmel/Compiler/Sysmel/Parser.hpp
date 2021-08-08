#ifndef SYSMEL_BASE_COMPILER_PARSER_PARSER_HPP
#define SYSMEL_BASE_COMPILER_PARSER_PARSER_HPP
#pragma once

#include "AST.hpp"
#include "Scanner.hpp"

namespace SysmelMoebius
{
namespace Compiler
{
namespace Sysmel
{

ASTNodePtr parseTokenList(const TokenListPtr &tokenList);
ASTNodePtr parseTokenListWithLiteralArrayContent(const TokenListPtr &tokenList);

inline ASTNodePtr parseString(const std::string &sourceString, const std::string &sourceName = "")
{
    return parseTokenList(scanString(sourceString, sourceName));
}

inline ASTNodePtr parseStringWithLiteralArrayContent(const std::string &sourceString, const std::string &sourceName = "")
{
    return parseTokenListWithLiteralArrayContent(scanString(sourceString, sourceName));
}

} // End of namespace Sysmel
} // End of namespace Compiler
} // End of namespace SysmelMoebius

#endif //SYSMEL_BASE_COMPILER_PARSER_PARSER_HPP