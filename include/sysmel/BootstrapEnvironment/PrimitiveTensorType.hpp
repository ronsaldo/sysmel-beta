#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_TENSOR_TYPE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_TENSOR_TYPE_HPP
#pragma once

#include "PrimitiveType.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(PrimitiveTensorType);

/**
 * I am a primitive tensor type. I designed to be directly supported by the cpu vectorial units.
 */
class PrimitiveTensorType : public SubtypeOf<PrimitiveType, PrimitiveTensorType>
{
public:
    static constexpr char const __typeName__[] = "PrimitiveTensorType";
    static constexpr char const __sysmelTypeName__[] = "_PrimitiveTensorType";

    virtual bool isPrimitiveTensorTypeValue() const override;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_TENSOR_TYPE_HPP