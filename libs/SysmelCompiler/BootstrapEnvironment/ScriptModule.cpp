#include "sysmel/BootstrapEnvironment/ScriptModule.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{
static BootstrapTypeRegistration<ScriptModule> scriptModuleTypeRegistration;

bool ScriptModule::isScriptModule() const
{
    return true;
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius