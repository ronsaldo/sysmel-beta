#ifndef SYSMEL_ENVIRONMENT_VALUE_BOX_HPP
#define SYSMEL_ENVIRONMENT_VALUE_BOX_HPP
#pragma once

#include "CompilerObject.hpp"

namespace Sysmel
{
namespace Environment
{

/**
 * I am a box that is used for holding a value. I am typically used for storing mutable variable values.
 */
class ValueBox : public SubtypeOf<CompilerObject, ValueBox>
{
public:
    static constexpr char const __typeName__[] = "ValueBox";

    virtual bool isValueBox() const override;
    virtual AnyValuePtr copyAssignValue(const AnyValuePtr &newValue) override;
    
    virtual AnyValuePtr accessVariableAsReferenceWithType(const TypePtr &referenceType) override;
    virtual AnyValuePtr accessVariableAsValueWithType(const TypePtr &valueType) override;

    virtual AnyValuePtr asMutableStoreValue() override;

    AnyValuePtr value;
    TypePtr type;
};

} // End of namespace Environment
} // End of namespace Sysmel

#endif //SYSMEL_ENVIRONMENT_VALUE_BOX_HPP