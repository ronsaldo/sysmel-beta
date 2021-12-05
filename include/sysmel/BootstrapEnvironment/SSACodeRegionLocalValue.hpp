#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SSA_CODE_REGION_LOCAL_VALUE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SSA_CODE_REGION_LOCAL_VALUE_HPP
#pragma once

#include "SSAValue.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

struct SSACodeRegionLocalValueEnumerationState
{
    uint32_t localValueCount = 0;
    uint32_t basicBlockCount = 0;
};

/**
 * I am the base interface for any object is specifically defined in the compiler domain.
 */
class SSACodeRegionLocalValue : public SubtypeOf<SSAValue, SSACodeRegionLocalValue>
{
public:
    static constexpr char const __typeName__[] = "SSACodeRegionLocalValue";

    virtual bool isSSACodeRegionLocalValue() const override;

    virtual SExpression asSExpression() const override;

    virtual void enumerateLocalValues(SSACodeRegionLocalValueEnumerationState &state);
protected:
    uint32_t localValueIndex;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_SSA_CODE_REGION_LOCAL_VALUE_HPP