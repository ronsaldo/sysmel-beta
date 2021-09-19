#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SEXPRESSION_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SEXPRESSION_HPP
#pragma once

#include "LargeInteger.hpp"
#include "Fraction.hpp"
#include <cstddef>
#include <variant>
#include <memory>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

struct SExpressionIdentifier
{
    std::string value;
};

struct SExpressionSymbol
{
    std::string value;
};

struct SExpressionList;

struct SExpressionVoid {};

typedef std::variant<std::nullptr_t, bool, LargeInteger, Fraction, double, char32_t, SExpressionVoid, std::string, SExpressionIdentifier, SExpressionSymbol, SExpressionList> SExpression;

struct SExpressionList
{
    std::vector<SExpression> elements;
};

std::string sexpressionToString(const SExpression &sexpr);
std::string sexpressionToPrettyString(const SExpression &sexpr);

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SEXPRESSION_HPP