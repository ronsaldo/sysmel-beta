#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_METABUILDER_FACTORY_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_METABUILDER_FACTORY_HPP
#pragma once

#include "BootstrapMethod.hpp"
#include "MacroInvocationContext.hpp"
#include "MetaBuilderInstanceContext.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

template<typename T>
MethodPtr metaBuilderFactoryFor (const std::string &name)
{
    return makeBootstrapMethod<std::shared_ptr<T> (MacroInvocationContextPtr)> (name, [](const MacroInvocationContextPtr &macroContext) {
        auto context = std::make_shared<MetaBuilderInstanceContext> ();
        context->instanceNode = macroContext->receiverNode;

        auto result = std::make_shared<T> ();
        result->setMetaBuilderInstanceContext(context);
        return result;
    });
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_METABUILDER_FACTORY_HPP