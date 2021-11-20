#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_BOOLEAN_TYPE_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_BOOLEAN_TYPE_HPP
#pragma once

#include "PrimitiveScalarType.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

SYSMEL_DECLARE_BOOTSTRAP_CLASS(PrimitiveBooleanType);
SYSMEL_DECLARE_BOOTSTRAP_CLASS(Boolean8);

/**
 * I am a primitive boolean type.
 */
class PrimitiveBooleanType : public SubtypeOf<PrimitiveScalarType, PrimitiveBooleanType>
{
public:
    static constexpr char const __typeName__[] = "PrimitiveBooleanType";
    static constexpr char const __sysmelTypeName__[] = "_PrimitiveBooleanType";

    virtual bool isPrimitiveBooleanTypeValue() const override;
};

/**
 * I am a 8 bits boolean value.
 */
class Boolean8 : public SubtypeOf<PrimitiveBooleanType, Boolean8>
{
public:
    typedef bool ValueType;
    
    static constexpr char const __typeName__[] = "Boolean8";
    static constexpr char const __sysmelTypeName__[] = "Boolean8";

    static Boolean8Ptr make(bool value);

    bool value = false;
};

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_PRIMITIVE_BOOLEAN_TYPE_HPP