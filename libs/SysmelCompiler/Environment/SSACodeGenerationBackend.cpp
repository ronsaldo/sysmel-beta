#include "Environment/SSACodeGenerationBackend.hpp"
#include "Environment/SubclassResponsibility.hpp"
#include "Environment/BootstrapTypeRegistration.hpp"

namespace Sysmel
{
namespace Environment
{

static BootstrapTypeRegistration<SSACodeGenerationBackend> SSACodeGenerationBackendTypeRegistration;

#ifndef BUILD_WITH_LLVM_BACKEND
SSACodeGenerationBackendPtr SSACodeGenerationBackend::makeNativeBackend()
{
    return nullptr;
}
#endif

SSACodeGenerationBackendPtr SSACodeGenerationBackend::makeSpirVBackend()
{
    return nullptr;
}

SSACodeGenerationOutputMode SSACodeGenerationBackend::getOutputMode() const
{
    return outputMode;
}

void SSACodeGenerationBackend::setOutputMode(SSACodeGenerationOutputMode newOutputMode)
{
    outputMode = newOutputMode;
}

DebugInformationType SSACodeGenerationBackend::getDebugInformationType()
{
    return debugInformationType;
}

void SSACodeGenerationBackend::setDebugInformationType(DebugInformationType type)
{
    debugInformationType = type;
}

void SSACodeGenerationBackend::setMainInputFileName(const std::string &newInputFileName)
{
    mainInputFileName = newInputFileName;
}

void SSACodeGenerationBackend::setOutputFileName(const std::string &newOutputFileName)
{
    outputFileName = newOutputFileName;
}

void SSACodeGenerationBackend::setEmitTargetIR(bool newEmitTargetIR)
{
    emitTargetIR = newEmitTargetIR;
}

bool SSACodeGenerationBackend::processAndWriteProgramModule(const ProgramModulePtr &programModule)
{
    (void)programModule;
    SysmelSelfSubclassResponsibility();
}

} // End of namespace Environment
} // End of namespace Sysmel