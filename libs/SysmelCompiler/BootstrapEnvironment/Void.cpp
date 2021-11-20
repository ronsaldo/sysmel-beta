#include "sysmel/BootstrapEnvironment/Void.hpp"
#include "sysmel/BootstrapEnvironment/Type.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"
#include <sstream>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{
static BootstrapTypeRegistration<Void> voidRegistration;

TypePtr Type::getVoidType()
{
    return Void::__staticType__();
}

AnyValuePtr getVoidConstant()
{
    return Void::uniqueInstance();
}

VoidPtr Void::uniqueInstance()
{
    auto singleton = std::make_shared<Void> ();
    return singleton;
}

bool Void::isVoid() const
{
    return true;
}

std::string Void::printString() const
{
    return "void";
}

SExpression Void::asSExpression() const
{
    return SExpressionVoid{};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius